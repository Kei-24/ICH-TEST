#ifndef LOGVIEWPAGE_H
#define LOGVIEWPAGE_H

#include <QWidget>
#include <QTextEdit>

class LogViewPage : public QWidget
{
    Q_OBJECT
public:
    explicit LogViewPage(QWidget *parent = nullptr);
    void refreshData();

private:
    QTextEdit *m_logText;
};

#endif // LOGVIEWPAGE_H
