#ifndef FAVORITEPAGE_H
#define FAVORITEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class FavoritePage : public QWidget
{
    Q_OBJECT
public:
    explicit FavoritePage(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onAddFavorite();
    void onRemoveFavorite();

private:
    QTableWidget *m_table;
    QLineEdit    *m_idEdit;
    QLabel       *m_msgLabel;
};

#endif // FAVORITEPAGE_H
