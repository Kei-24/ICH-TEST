#include "usermanagepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

extern "C" {
#include "app_init.h"
#include "user_service.h"
#include "linkedlist.h"
}

UserManagePage::UserManagePage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel(QStringLiteral("用户管理"));
    QFont f; f.setPointSize(14); f.setBold(true);
    title->setFont(f);
    layout->addWidget(title);

    /* 添加用户区域 */
    auto *addGroup = new QGroupBox(QStringLiteral("添加用户"));
    auto *addLayout = new QHBoxLayout(addGroup);
    auto *addForm = new QFormLayout;

    m_nameEdit = new QLineEdit;
    m_nameEdit->setPlaceholderText(QStringLiteral("显示名称"));
    addForm->addRow(QStringLiteral("用户名："), m_nameEdit);

    m_accountEdit = new QLineEdit;
    m_accountEdit->setPlaceholderText(QStringLiteral("登录账号"));
    addForm->addRow(QStringLiteral("账号："), m_accountEdit);

    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText(QStringLiteral("登录密码"));
    addForm->addRow(QStringLiteral("密码："), m_passwordEdit);

    addLayout->addLayout(addForm);
    auto *addBtn = new QPushButton(QStringLiteral("添加"));
    addBtn->setFixedHeight(60);
    addLayout->addWidget(addBtn);
    layout->addWidget(addGroup);

    /* 删除用户区域 */
    auto *delGroup = new QGroupBox(QStringLiteral("删除用户"));
    auto *delLayout = new QHBoxLayout(delGroup);
    m_delAccountEdit = new QLineEdit;
    m_delAccountEdit->setPlaceholderText(QStringLiteral("输入要删除的账号"));
    delLayout->addWidget(new QLabel(QStringLiteral("账号：")));
    delLayout->addWidget(m_delAccountEdit);
    auto *delBtn = new QPushButton(QStringLiteral("删除（软删除）"));
    delLayout->addWidget(delBtn);
    layout->addWidget(delGroup);

    /* 提示信息 */
    m_msgLabel = new QLabel;
    m_msgLabel->setStyleSheet("color: green;");
    layout->addWidget(m_msgLabel);

    /* 用户列表 */
    m_table = new QTableWidget;
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("用户名"), QStringLiteral("账号"),
        QStringLiteral("身份"), QStringLiteral("状态")
    });
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setAlternatingRowColors(true);
    layout->addWidget(m_table);

    connect(addBtn, &QPushButton::clicked, this, &UserManagePage::onAddUser);
    connect(delBtn, &QPushButton::clicked, this, &UserManagePage::onDeleteUser);
}

void UserManagePage::refreshData()
{
    m_table->setRowCount(0);

    User *cur = g_ctx.userHead;
    while (cur) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8(cur->displayName)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(cur->account)));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(user_role_to_str(cur->role))));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(user_status_to_str(cur->status))));
        cur = cur->next;
    }

    m_table->resizeColumnsToContents();
}

void UserManagePage::onAddUser()
{
    QString name = m_nameEdit->text().trimmed();
    QString account = m_accountEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (name.isEmpty() || account.isEmpty() || password.isEmpty()) {
        m_msgLabel->setStyleSheet("color: red;");
        m_msgLabel->setText(QStringLiteral("请填写完整的用户信息"));
        return;
    }

    QByteArray nb = name.toUtf8();
    QByteArray ab = account.toUtf8();
    QByteArray pb = password.toUtf8();

    if (user_add(nb.constData(), ab.constData(), pb.constData())) {
        m_msgLabel->setStyleSheet("color: green;");
        m_msgLabel->setText(QStringLiteral("用户添加成功！"));
        m_nameEdit->clear();
        m_accountEdit->clear();
        m_passwordEdit->clear();
        refreshData();
    } else {
        m_msgLabel->setStyleSheet("color: red;");
        m_msgLabel->setText(QStringLiteral("添加失败（账号已存在或含非法字符）"));
    }
}

void UserManagePage::onDeleteUser()
{
    QString account = m_delAccountEdit->text().trimmed();
    if (account.isEmpty()) {
        m_msgLabel->setStyleSheet("color: red;");
        m_msgLabel->setText(QStringLiteral("请输入要删除的账号"));
        return;
    }

    QByteArray ab = account.toUtf8();
    if (user_delete(ab.constData())) {
        m_msgLabel->setStyleSheet("color: green;");
        m_msgLabel->setText(QStringLiteral("用户已注销！"));
        m_delAccountEdit->clear();
        refreshData();
    } else {
        m_msgLabel->setStyleSheet("color: red;");
        m_msgLabel->setText(QStringLiteral("删除失败（不存在/管理员不可删除/已注销）"));
    }
}
