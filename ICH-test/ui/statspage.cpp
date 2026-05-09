#include "statspage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>

extern "C" {
#include "app_init.h"
#include "archive_service.h"
}

StatsPage::StatsPage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel(QStringLiteral("排序与统计"));
    QFont f; f.setPointSize(14); f.setBold(true);
    title->setFont(f);
    layout->addWidget(title);

    /* 按钮栏 */
    auto *btnBar = new QHBoxLayout;
    auto *heatBtn = new QPushButton(QStringLiteral("国际热度 Top10"));
    auto *compBtn = new QPushButton(QStringLiteral("综合传播热度 Top10"));
    auto *catBtn  = new QPushButton(QStringLiteral("类别占比统计"));
    btnBar->addWidget(heatBtn);
    btnBar->addWidget(compBtn);
    btnBar->addWidget(catBtn);
    btnBar->addStretch();
    layout->addLayout(btnBar);

    m_titleLabel = new QLabel;
    QFont tf; tf.setPointSize(12); tf.setBold(true);
    m_titleLabel->setFont(tf);
    layout->addWidget(m_titleLabel);

    /* 表格 */
    m_table = new QTableWidget;
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    layout->addWidget(m_table);

    /* 统计文字 */
    m_statsLabel = new QLabel;
    m_statsLabel->setWordWrap(true);
    m_statsLabel->setAlignment(Qt::AlignTop);
    layout->addWidget(m_statsLabel);

    connect(heatBtn, &QPushButton::clicked, this, &StatsPage::showHeatRanking);
    connect(compBtn, &QPushButton::clicked, this, &StatsPage::showCompositeRanking);
    connect(catBtn,  &QPushButton::clicked, this, &StatsPage::showCategoryStats);
}

void StatsPage::refreshData()
{
    showHeatRanking();
}

void StatsPage::showHeatRanking()
{
    m_statsLabel->clear();
    m_titleLabel->setText(QStringLiteral("国际热度指数排名 Top 10"));

    m_table->clear();
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("排名"), QStringLiteral("编号"), QStringLiteral("名称"),
        QStringLiteral("热度指数"), QStringLiteral("收藏量")
    });
    m_table->setRowCount(0);

    int count = 0;
    Archive **arr = archive_sort_by_heat(&count);
    if (!arr) return;

    int show = count > 10 ? 10 : count;
    for (int i = 0; i < show; i++) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(arr[i]->id)));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(arr[i]->name)));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(arr[i]->heatIndex)));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::number(arr[i]->favoriteCount)));
    }

    free(arr);
    m_table->resizeColumnsToContents();
}

void StatsPage::showCompositeRanking()
{
    m_statsLabel->clear();
    m_titleLabel->setText(QStringLiteral("综合传播热度排名 Top 10"));

    m_table->clear();
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("排名"), QStringLiteral("编号"), QStringLiteral("名称"),
        QStringLiteral("热度指数"), QStringLiteral("收藏量"), QStringLiteral("综合热度")
    });
    m_table->setRowCount(0);

    int count = 0;
    Archive **arr = archive_sort_by_composite_heat(&count);
    if (!arr) return;

    int show = count > 10 ? 10 : count;
    for (int i = 0; i < show; i++) {
        long long comp = (long long)arr[i]->heatIndex * arr[i]->favoriteCount;
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(arr[i]->id)));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(arr[i]->name)));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(arr[i]->heatIndex)));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::number(arr[i]->favoriteCount)));
        m_table->setItem(row, 5, new QTableWidgetItem(QString::number(comp)));
    }

    free(arr);
    m_table->resizeColumnsToContents();
}

void StatsPage::showCategoryStats()
{
    m_titleLabel->setText(QStringLiteral("各类别非遗项目数量统计"));
    m_table->clear();
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("类别"), QStringLiteral("数量"), QStringLiteral("占比")
    });
    m_table->setRowCount(0);

    int result[CAT_COUNT];
    archive_stat_category(result);
    int total = archive_stat_total_active();

    QString statsText;
    for (int i = 0; i < CAT_COUNT; i++) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        double pct = total > 0 ? (result[i] * 100.0 / total) : 0;
        m_table->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(category_to_str((CultureCategory)i))));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(result[i])));
        m_table->setItem(row, 2, new QTableWidgetItem(QStringLiteral("%1%").arg(pct, 0, 'f', 1)));

        statsText += QStringLiteral("%1: %2 项 (%3%)\n")
                     .arg(QString::fromUtf8(category_to_str((CultureCategory)i)))
                     .arg(result[i])
                     .arg(pct, 0, 'f', 1);
    }
    statsText += QStringLiteral("\n有效档案总数: %1").arg(total);

    m_statsLabel->setText(statsText);
    m_table->resizeColumnsToContents();
}
