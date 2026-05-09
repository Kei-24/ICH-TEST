#ifndef STATSPAGE_H
#define STATSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>

class StatsPage : public QWidget
{
    Q_OBJECT
public:
    explicit StatsPage(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void showHeatRanking();
    void showCompositeRanking();
    void showCategoryStats();

private:
    QTableWidget *m_table;
    QLabel       *m_titleLabel;
    QLabel       *m_statsLabel;  // 类别统计文字
};

#endif // STATSPAGE_H
