#include "favoritepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

extern "C" {
#include "app_init.h"
#include "favorite_service.h"
#include "linkedlist.h"
}

FavoritePage::FavoritePage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    /* 标题 */
    auto *title = new QLabel(QStringLiteral("我的文化护照（个人收藏）"));
    QFont f; f.setPointSize(14); f.setBold(true);
    title->setFont(f);
    layout->addWidget(title);

    /* 操作栏 */
    auto *opBar = new QHBoxLayout;
    opBar->addWidget(new QLabel(QStringLiteral("项目编号：")));
    m_idEdit = new QLineEdit;
    m_idEdit->setPlaceholderText(QStringLiteral("输入要收藏/取消的项目编号"));
    m_idEdit->setMaximumWidth(200);
    opBar->addWidget(m_idEdit);

    auto *addBtn = new QPushButton(QStringLiteral("收藏"));
    auto *removeBtn = new QPushButton(QStringLiteral("取消收藏"));
    opBar->addWidget(addBtn);
    opBar->addWidget(removeBtn);
    opBar->addStretch();

    m_msgLabel = new QLabel;
    m_msgLabel->setStyleSheet("color: green;");
    opBar->addWidget(m_msgLabel);
    layout->addLayout(opBar);

    /* 收藏表格 */
    m_table = new QTableWidget;
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("项目编号"), QStringLiteral("项目名称"),
        QStringLiteral("文化类别"), QStringLiteral("热度指数"),
        QStringLiteral("收藏时间")
    });
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    layout->addWidget(m_table);

    connect(addBtn, &QPushButton::clicked, this, &FavoritePage::onAddFavorite);
    connect(removeBtn, &QPushButton::clicked, this, &FavoritePage::onRemoveFavorite);
}

void FavoritePage::refreshData()
{
    m_table->setRowCount(0);

    int count = 0;
    Favorite **favs = favorite_list_by_current_user(&count);
    if (!favs) return;

    for (int i = 0; i < count; i++) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(favs[i]->archiveId)));

        /* 查找档案名称等信息 */
        Archive *a = archive_node_find_by_id(g_ctx.archiveHead, favs[i]->archiveId);
        if (a) {
            m_table->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(a->name)));
            m_table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(category_to_str(a->category))));
            m_table->setItem(row, 3, new QTableWidgetItem(QString::number(a->heatIndex)));
        } else {
            m_table->setItem(row, 1, new QTableWidgetItem(QStringLiteral("(已删除)")));
            m_table->setItem(row, 2, new QTableWidgetItem(QStringLiteral("-")));
            m_table->setItem(row, 3, new QTableWidgetItem(QStringLiteral("-")));
        }
        m_table->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(favs[i]->time)));
    }

    free(favs);
    m_table->resizeColumnsToContents();
}

void FavoritePage::onAddFavorite()
{
    int id = m_idEdit->text().trimmed().toInt();
    if (id <= 0) {
        m_msgLabel->setStyleSheet("color: red;");
        m_msgLabel->setText(QStringLiteral("请输入有效的项目编号"));
        return;
    }

    if (favorite_add(id)) {
        m_msgLabel->setStyleSheet("color: green;");
        m_msgLabel->setText(QStringLiteral("收藏成功！"));
        refreshData();
    } else {
        m_msgLabel->setStyleSheet("color: red;");
        m_msgLabel->setText(QStringLiteral("收藏失败（项目不存在或已收藏）"));
    }
    m_idEdit->clear();
}

void FavoritePage::onRemoveFavorite()
{
    int id = m_idEdit->text().trimmed().toInt();
    if (id <= 0) {
        m_msgLabel->setStyleSheet("color: red;");
        m_msgLabel->setText(QStringLiteral("请输入有效的项目编号"));
        return;
    }

    if (favorite_remove(id)) {
        m_msgLabel->setStyleSheet("color: green;");
        m_msgLabel->setText(QStringLiteral("取消收藏成功！"));
        refreshData();
    } else {
        m_msgLabel->setStyleSheet("color: red;");
        m_msgLabel->setText(QStringLiteral("取消失败（未收藏该项目）"));
    }
    m_idEdit->clear();
}
