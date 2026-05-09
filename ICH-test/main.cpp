#include "mainwindow.h"
#include "ui/loginwindow.h"

#include <QApplication>

extern "C" {
#include "app_init.h"
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /* 初始化核心数据层 */
    if (!app_init()) {
        return -1;
    }

    auto *loginWindow = new LoginWindow;
    auto *mainWindow  = new MainWindow;

    /* 登录成功 -> 隐藏登录窗口，显示主窗口 */
    QObject::connect(loginWindow, &LoginWindow::loginSuccess, [=]() {
        loginWindow->hide();
        mainWindow->setupForRole();
        mainWindow->show();
    });

    /* 退出登录 -> 隐藏主窗口，显示登录窗口 */
    QObject::connect(mainWindow, &MainWindow::logoutRequested, [=]() {
        mainWindow->hide();
        loginWindow->show();
    });

    loginWindow->show();

    int ret = a.exec();

    delete mainWindow;
    delete loginWindow;

    app_shutdown();
    return ret;
}
