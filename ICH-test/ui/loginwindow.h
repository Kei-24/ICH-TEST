#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void loginSuccess();  // 登录成功后发出信号

private slots:
    void onLoginClicked();

private:
    QLineEdit *m_accountEdit;
    QLineEdit *m_passwordEdit;
    QLabel    *m_msgLabel;
    QPushButton *m_loginBtn;
};

#endif // LOGINWINDOW_H
