#include "mainwindow.h"
#include "ui/archivepage.h"
#include "ui/archiveeditdialog.h"
#include "ui/favoritepage.h"
#include "ui/usermanagepage.h"
#include "ui/statspage.h"
#include "ui/logviewpage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFont>

extern "C" {
#include "app_init.h"
#include "user_service.h"
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_archivePage(nullptr)
    , m_favoritePage(nullptr)
    , m_userManagePage(nullptr)
    , m_statsPage(nullptr)
    , m_logViewPage(nullptr)
{
    setWindowTitle(QStringLiteral("非遗文化数字化传承与展示平台"));
    resize(1100, 700);

    auto *central = new QWidget;
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    /* 左侧导航面板 */
    auto *leftPanel = new QWidget;
    leftPanel->setFixedWidth(180);
    leftPanel->setStyleSheet("background-color: #2c3e50;");
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 10, 0, 10);

    m_userLabel = new QLabel;
    m_userLabel->setAlignment(Qt::AlignCenter);
    m_userLabel->setStyleSheet("color: white; padding: 10px; font-size: 13px;");
    leftLayout->addWidget(m_userLabel);

    m_menu = new QListWidget;
    m_menu->setStyleSheet(
        "QListWidget { background: #2c3e50; color: white; border: none; font-size: 14px; }"
        "QListWidget::item { padding: 12px 15px; }"
        "QListWidget::item:selected { background: #3498db; }"
        "QListWidget::item:hover { background: #34495e; }"
    );
    leftLayout->addWidget(m_menu, 1);

    mainLayout->addWidget(leftPanel);

    /* 右侧页面区域 */
    m_stack = new QStackedWidget;
    m_stack->setStyleSheet("background: white;");
    mainLayout->addWidget(m_stack, 1);

    setCentralWidget(central);

    connect(m_menu, &QListWidget::currentRowChanged, this, &MainWindow::onMenuClicked);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupForRole()
{
    clearPages();
    m_menu->clear();

    if (!g_ctx.currentUser) return;

    bool isAdmin = (g_ctx.currentUser->role == ROLE_ADMIN);

    m_userLabel->setText(QStringLiteral("欢迎，%1\n(%2)")
        .arg(QString::fromUtf8(g_ctx.currentUser->displayName))
        .arg(isAdmin ? QStringLiteral("管理员") : QStringLiteral("普通用户")));

    if (isAdmin) {
        /* 管理员菜单 */
        m_menu->addItem(QStringLiteral("  档案浏览/查询"));    // 0
        m_menu->addItem(QStringLiteral("  用户管理"));          // 1
        m_menu->addItem(QStringLiteral("  排序与统计"));        // 2
        m_menu->addItem(QStringLiteral("  系统日志"));          // 3
        m_menu->addItem(QStringLiteral("  退出登录"));          // 4

        m_archivePage = new ArchivePage(true);
        m_userManagePage = new UserManagePage;
        m_statsPage = new StatsPage;
        m_logViewPage = new LogViewPage;

        m_stack->addWidget(m_archivePage);    // 0
        m_stack->addWidget(m_userManagePage); // 1
        m_stack->addWidget(m_statsPage);      // 2
        m_stack->addWidget(m_logViewPage);    // 3

        connect(m_archivePage, &ArchivePage::requestEdit, this, &MainWindow::onEditArchive);
        connect(m_archivePage, &ArchivePage::requestAdd,  this, &MainWindow::onAddArchive);
    } else {
        /* 普通用户菜单 */
        m_menu->addItem(QStringLiteral("  档案浏览/查询"));    // 0
        m_menu->addItem(QStringLiteral("  我的文化护照"));      // 1
        m_menu->addItem(QStringLiteral("  退出登录"));          // 2

        m_archivePage = new ArchivePage(false);
        m_favoritePage = new FavoritePage;

        m_stack->addWidget(m_archivePage);   // 0
        m_stack->addWidget(m_favoritePage);  // 1
    }

    m_menu->setCurrentRow(0);
}

void MainWindow::clearPages()
{
    /* 移除并销毁所有页面 */
    while (m_stack->count() > 0) {
        QWidget *w = m_stack->widget(0);
        m_stack->removeWidget(w);
        w->deleteLater();
    }

    m_archivePage = nullptr;
    m_favoritePage = nullptr;
    m_userManagePage = nullptr;
    m_statsPage = nullptr;
    m_logViewPage = nullptr;
}

void MainWindow::onMenuClicked(int row)
{
    if (row < 0) return;

    bool isAdmin = g_ctx.currentUser && (g_ctx.currentUser->role == ROLE_ADMIN);

    /* 检查是否点击了退出 */
    int logoutIdx = isAdmin ? 4 : 2;
    if (row == logoutIdx) {
        user_logout();
        emit logoutRequested();
        return;
    }

    /* 切换到对应页面 */
    if (row < m_stack->count()) {
        m_stack->setCurrentIndex(row);

        /* 每次切换时刷新页面数据 */
        if (isAdmin) {
            switch (row) {
            case 0: m_archivePage->refreshData(); break;
            case 1: m_userManagePage->refreshData(); break;
            case 2: m_statsPage->refreshData(); break;
            case 3: m_logViewPage->refreshData(); break;
            }
        } else {
            switch (row) {
            case 0: m_archivePage->refreshData(); break;
            case 1: m_favoritePage->refreshData(); break;
            }
        }
    }
}

void MainWindow::onEditArchive(int archiveId)
{
    ArchiveEditDialog dlg(archiveId, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_archivePage->refreshData();
    }
}

void MainWindow::onAddArchive()
{
    ArchiveEditDialog dlg(0, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_archivePage->refreshData();
    }
}
