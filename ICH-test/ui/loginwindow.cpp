#include "loginwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

extern "C" {
#include "user_service.h"
}

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(400, 300);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(15);

    /* 标题 */
    auto *titleLabel = new QLabel(QStringLiteral("非遗文化数字化传承平台"));
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(10);

    /* 账号 */
    auto *accLayout = new QHBoxLayout;
    accLayout->addWidget(new QLabel(QStringLiteral("账号：")));
    m_accountEdit = new QLineEdit;
    m_accountEdit->setPlaceholderText(QStringLiteral("请输入账号"));
    m_accountEdit->setFixedWidth(200);
    accLayout->addWidget(m_accountEdit);
    mainLayout->addLayout(accLayout);

    /* 密码 */
    auto *pwdLayout = new QHBoxLayout;
    pwdLayout->addWidget(new QLabel(QStringLiteral("密码：")));
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText(QStringLiteral("请输入密码"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setFixedWidth(200);
    pwdLayout->addWidget(m_passwordEdit);
    mainLayout->addLayout(pwdLayout);

    /* 提示信息 */
    m_msgLabel = new QLabel;
    m_msgLabel->setStyleSheet("color: red;");
    m_msgLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_msgLabel);

    /* 登录按钮 */
    m_loginBtn = new QPushButton(QStringLiteral("登 录"));
    m_loginBtn->setFixedSize(120, 36);
    auto *btnLayout = new QHBoxLayout;
    btnLayout->setAlignment(Qt::AlignCenter);
    btnLayout->addWidget(m_loginBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked()
{
    QString account = m_accountEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (account.isEmpty() || password.isEmpty()) {
        m_msgLabel->setText(QStringLiteral("请输入账号和密码"));
        return;
    }

    QByteArray accBytes = account.toUtf8();
    QByteArray pwdBytes = password.toUtf8();

    User *user = user_login(accBytes.constData(), pwdBytes.constData());
    if (user) {
        m_msgLabel->clear();
        emit loginSuccess();
    } else {
        m_msgLabel->setText(QStringLiteral("账号或密码错误，或账号已被封禁"));
    }
}
