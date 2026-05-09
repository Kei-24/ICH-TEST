#ifndef USERMANAGEPAGE_H
#define USERMANAGEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>

class UserManagePage : public QWidget
{
    Q_OBJECT
public:
    explicit UserManagePage(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onAddUser();
    void onDeleteUser();

private:
    QTableWidget *m_table;
    QLineEdit *m_nameEdit;
    QLineEdit *m_accountEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_delAccountEdit;
    QLabel    *m_msgLabel;
};

#endif // USERMANAGEPAGE_H
