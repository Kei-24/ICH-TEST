#include "archivepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPixmap>
#include <QFileInfo>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollArea>

extern "C" {
#include "app_init.h"
#include "archive_service.h"
#include "linkedlist.h"
#include "favorite_service.h"
}

ArchivePage::ArchivePage(bool isAdmin, QWidget *parent)
    : QWidget(parent), m_isAdmin(isAdmin), m_currentPage(0), m_pageSize(10), m_totalPages(0)
{
    auto *mainLayout = new QVBoxLayout(this);

    /* 顶部工具栏：搜索 + 添加按钮 */
    auto *topBar = new QHBoxLayout;
    topBar->addWidget(new QLabel(QStringLiteral("搜索：")));
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText(QStringLiteral("输入项目名称关键字..."));
    m_searchEdit->setMaximumWidth(300);
    topBar->addWidget(m_searchEdit);
    topBar->addStretch();

    m_addBtn = new QPushButton(QStringLiteral("＋ 添加档案"));
    m_addBtn->setVisible(m_isAdmin);
    topBar->addWidget(m_addBtn);

    mainLayout->addLayout(topBar);

    /* 中间：表格 + 详情面板（水平分割） */
    auto *splitter = new QSplitter(Qt::Horizontal);

    /* 左侧表格 */
    m_table = new QTableWidget;
    m_table->setColumnCount(11);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("编号"), QStringLiteral("名称"), QStringLiteral("类别"),
        QStringLiteral("发源地"), QStringLiteral("传承人"), QStringLiteral("保护级别"),
        QStringLiteral("数字化"), QStringLiteral("热度"), QStringLiteral("收藏量"),
        QStringLiteral("综合热度"), QStringLiteral("状态")
    });
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    splitter->addWidget(m_table);

    /* 右侧详情面板 */
    auto *detailWidget = new QWidget;
    auto *detailLayout = new QVBoxLayout(detailWidget);

    m_imageLabel = new QLabel;
    m_imageLabel->setFixedSize(200, 200);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet("border: 1px solid #ccc; background: #f5f5f5;");
    m_imageLabel->setText(QStringLiteral("暂无图片"));
    detailLayout->addWidget(m_imageLabel, 0, Qt::AlignHCenter);

    m_detailLabel = new QLabel;
    m_detailLabel->setWordWrap(true);
    m_detailLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_detailLabel->setText(QStringLiteral("点击左侧表格中的档案查看详情"));
    detailLayout->addWidget(m_detailLabel);

    m_editBtn = new QPushButton(QStringLiteral("编辑此档案"));
    m_editBtn->setVisible(false);
    detailLayout->addWidget(m_editBtn);

    detailLayout->addStretch();
    detailWidget->setMinimumWidth(250);
    detailWidget->setMaximumWidth(350);
    splitter->addWidget(detailWidget);

    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter, 1);

    /* 底部分页栏 */
    auto *pageBar = new QHBoxLayout;
    auto *btnFirst = new QPushButton(QStringLiteral("首页"));
    auto *btnPrev  = new QPushButton(QStringLiteral("上一页"));
    auto *btnNext  = new QPushButton(QStringLiteral("下一页"));
    auto *btnLast  = new QPushButton(QStringLiteral("末页"));
    m_pageLabel = new QLabel;
    pageBar->addStretch();
    pageBar->addWidget(btnFirst);
    pageBar->addWidget(btnPrev);
    pageBar->addWidget(m_pageLabel);
    pageBar->addWidget(btnNext);
    pageBar->addWidget(btnLast);
    pageBar->addStretch();
    mainLayout->addLayout(pageBar);

    /* 连接信号 */
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ArchivePage::onSearchTextChanged);
    connect(btnFirst, &QPushButton::clicked, this, &ArchivePage::onFirstPage);
    connect(btnPrev,  &QPushButton::clicked, this, &ArchivePage::onPrevPage);
    connect(btnNext,  &QPushButton::clicked, this, &ArchivePage::onNextPage);
    connect(btnLast,  &QPushButton::clicked, this, &ArchivePage::onLastPage);
    connect(m_table, &QTableWidget::cellClicked, this, &ArchivePage::onRowClicked);
    connect(m_addBtn, &QPushButton::clicked, this, &ArchivePage::onAddClicked);
    connect(m_editBtn, &QPushButton::clicked, this, [this]() {
        int row = m_table->currentRow();
        if (row >= 0) {
            int id = m_table->item(row, 0)->text().toInt();
            emit requestEdit(id);
        }
    });

    refreshData();
}

void ArchivePage::refreshData()
{
    QString keyword = m_searchEdit->text().trimmed();
    m_filteredIds.clear();

    if (keyword.isEmpty()) {
        /* 显示所有NORMAL档案 */
        Archive *cur = g_ctx.archiveHead;
        while (cur) {
            if (cur->status == ASTAT_NORMAL)
                m_filteredIds.append(cur->id);
            cur = cur->next;
        }
    } else {
        QByteArray kw = keyword.toUtf8();
        int count = 0;
        Archive **results = archive_search_by_name(kw.constData(), &count);
        if (results) {
            for (int i = 0; i < count; i++)
                m_filteredIds.append(results[i]->id);
            free(results);
        }
    }

    m_totalPages = (m_filteredIds.size() + m_pageSize - 1) / m_pageSize;
    if (m_totalPages == 0) m_totalPages = 1;
    if (m_currentPage >= m_totalPages) m_currentPage = m_totalPages - 1;
    if (m_currentPage < 0) m_currentPage = 0;

    loadPage();
}

void ArchivePage::loadPage()
{
    m_table->setRowCount(0);

    int start = m_currentPage * m_pageSize;
    int end = qMin(start + m_pageSize, m_filteredIds.size());

    for (int i = start; i < end; i++) {
        Archive *a = archive_node_find_by_id(g_ctx.archiveHead, m_filteredIds[i]);
        if (!a) continue;

        int row = m_table->rowCount();
        m_table->insertRow(row);

        long long compositeHeat = (long long)a->heatIndex * a->favoriteCount;

        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(a->id)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(a->name)));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(category_to_str(a->category))));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(a->origin)));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(a->inheritor)));
        m_table->setItem(row, 5, new QTableWidgetItem(QString::fromUtf8(protection_to_str(a->protection))));
        m_table->setItem(row, 6, new QTableWidgetItem(QString::fromUtf8(digital_status_to_str(a->digitalStatus))));
        m_table->setItem(row, 7, new QTableWidgetItem(QString::number(a->heatIndex)));
        m_table->setItem(row, 8, new QTableWidgetItem(QString::number(a->favoriteCount)));
        m_table->setItem(row, 9, new QTableWidgetItem(QString::number(compositeHeat)));
        m_table->setItem(row, 10, new QTableWidgetItem(QString::fromUtf8(archive_status_to_str(a->status))));
    }

    m_table->resizeColumnsToContents();
    m_pageLabel->setText(QStringLiteral("第 %1/%2 页（共 %3 条）")
                         .arg(m_currentPage + 1).arg(m_totalPages).arg(m_filteredIds.size()));
}

void ArchivePage::showDetail(int archiveId)
{
    Archive *a = archive_node_find_by_id(g_ctx.archiveHead, archiveId);
    if (!a) return;

    long long compositeHeat = (long long)a->heatIndex * a->favoriteCount;

    QString detail = QStringLiteral(
        "<b>项目编号：</b>%1<br>"
        "<b>项目名称：</b>%2<br>"
        "<b>文化类别：</b>%3<br>"
        "<b>朝代/发源地：</b>%4<br>"
        "<b>传承人：</b>%5<br>"
        "<b>保护级别：</b>%6<br>"
        "<b>数字化状态：</b>%7<br>"
        "<b>国际热度指数：</b>%8<br>"
        "<b>收藏量：</b>%9<br>"
        "<b>综合传播热度：</b>%10"
    ).arg(a->id)
     .arg(QString::fromUtf8(a->name))
     .arg(QString::fromUtf8(category_to_str(a->category)))
     .arg(QString::fromUtf8(a->origin))
     .arg(QString::fromUtf8(a->inheritor))
     .arg(QString::fromUtf8(protection_to_str(a->protection)))
     .arg(QString::fromUtf8(digital_status_to_str(a->digitalStatus)))
     .arg(a->heatIndex)
     .arg(a->favoriteCount)
     .arg(compositeHeat);

    m_detailLabel->setText(detail);

    /* 加载封面图片 */
    QString imgPath = QStringLiteral("images/%1").arg(QString::fromUtf8(a->imagePath));
    if (QFileInfo::exists(imgPath)) {
        QPixmap pix(imgPath);
        m_imageLabel->setPixmap(pix.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_imageLabel->setText(QStringLiteral("暂无图片"));
        m_imageLabel->setPixmap(QPixmap());
    }

    /* 管理员可见编辑按钮 */
    m_editBtn->setVisible(m_isAdmin);
}

void ArchivePage::onSearchTextChanged(const QString &)
{
    m_currentPage = 0;
    refreshData();
}

void ArchivePage::onFirstPage() { m_currentPage = 0; loadPage(); }
void ArchivePage::onPrevPage()  { if (m_currentPage > 0) { m_currentPage--; loadPage(); } }
void ArchivePage::onNextPage()  { if (m_currentPage < m_totalPages - 1) { m_currentPage++; loadPage(); } }
void ArchivePage::onLastPage()  { m_currentPage = m_totalPages - 1; loadPage(); }

void ArchivePage::onRowClicked(int row, int)
{
    auto *item = m_table->item(row, 0);
    if (item) showDetail(item->text().toInt());
}

void ArchivePage::onAddClicked()
{
    emit requestAdd();
}
