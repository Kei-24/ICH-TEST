#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>

class ArchivePage;
class FavoritePage;
class UserManagePage;
class StatsPage;
class LogViewPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    /* 登录成功后根据角色初始化菜单和页面 */
    void setupForRole();

signals:
    void logoutRequested();  // 退出登录

private slots:
    void onMenuClicked(int row);
    void onEditArchive(int archiveId);
    void onAddArchive();

private:
    void clearPages();

    QListWidget    *m_menu;
    QStackedWidget *m_stack;
    QLabel         *m_userLabel;

    ArchivePage    *m_archivePage;
    FavoritePage   *m_favoritePage;
    UserManagePage *m_userManagePage;
    StatsPage      *m_statsPage;
    LogViewPage    *m_logViewPage;
};

#endif // MAINWINDOW_H
