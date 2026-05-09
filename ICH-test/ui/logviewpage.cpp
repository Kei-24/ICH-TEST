#include "logviewpage.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>

extern "C" {
#include "logger.h"
#include "file_io.h"
}

LogViewPage::LogViewPage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *topBar = new QHBoxLayout;
    auto *title = new QLabel(QStringLiteral("系统操作日志"));
    QFont f; f.setPointSize(14); f.setBold(true);
    title->setFont(f);
    topBar->addWidget(title);
    topBar->addStretch();
    auto *refreshBtn = new QPushButton(QStringLiteral("刷新"));
    topBar->addWidget(refreshBtn);
    layout->addLayout(topBar);

    m_logText = new QTextEdit;
    m_logText->setReadOnly(true);
    m_logText->setFont(QFont("Consolas", 10));
    layout->addWidget(m_logText);

    connect(refreshBtn, &QPushButton::clicked, this, &LogViewPage::refreshData);
}

void LogViewPage::refreshData()
{
    char *content = log_read_all(LOG_FILE);
    if (content) {
        m_logText->setPlainText(QString::fromUtf8(content));
        free(content);
        /* 滚动到底部 */
        auto cursor = m_logText->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_logText->setTextCursor(cursor);
    } else {
        m_logText->setPlainText(QStringLiteral("暂无日志记录"));
    }
}
