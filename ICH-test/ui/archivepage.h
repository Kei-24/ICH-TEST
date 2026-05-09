#ifndef ARCHIVEPAGE_H
#define ARCHIVEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class ArchivePage : public QWidget
{
    Q_OBJECT
public:
    explicit ArchivePage(bool isAdmin, QWidget *parent = nullptr);
    void refreshData();

signals:
    void requestEdit(int archiveId);   // 管理员点击编辑
    void requestAdd();                 // 管理员点击添加

private slots:
    void onSearchTextChanged(const QString &text);
    void onFirstPage();
    void onPrevPage();
    void onNextPage();
    void onLastPage();
    void onRowClicked(int row, int col);
    void onAddClicked();

private:
    void loadPage();
    void showDetail(int archiveId);

    bool m_isAdmin;
    QTableWidget *m_table;
    QLineEdit    *m_searchEdit;
    QLabel       *m_pageLabel;
    QLabel       *m_detailLabel;
    QLabel       *m_imageLabel;
    QPushButton  *m_editBtn;
    QPushButton  *m_addBtn;

    /* 分页数据 */
    struct ArchiveItem { int id; };
    QVector<int> m_filteredIds;   // 当前筛选后的所有档案ID列表
    int m_currentPage;
    int m_pageSize;
    int m_totalPages;
};

#endif // ARCHIVEPAGE_H
