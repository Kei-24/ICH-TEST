#include "archiveeditdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>

extern "C" {
#include "app_init.h"
#include "archive_service.h"
#include "linkedlist.h"
}

ArchiveEditDialog::ArchiveEditDialog(int editId, QWidget *parent)
    : QDialog(parent), m_editId(editId)
{
    setWindowTitle(editId > 0 ? QStringLiteral("编辑档案") : QStringLiteral("添加档案"));
    setMinimumWidth(400);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout;

    m_nameEdit = new QLineEdit;
    form->addRow(QStringLiteral("项目名称："), m_nameEdit);

    m_categoryCombo = new QComboBox;
    m_categoryCombo->addItems({
        QStringLiteral("传统技艺"), QStringLiteral("传统音乐"), QStringLiteral("传统戏剧"),
        QStringLiteral("传统美术"), QStringLiteral("民俗"), QStringLiteral("传统医药")
    });
    form->addRow(QStringLiteral("文化类别："), m_categoryCombo);

    m_originEdit = new QLineEdit;
    form->addRow(QStringLiteral("朝代/发源地："), m_originEdit);

    m_inheritorEdit = new QLineEdit;
    form->addRow(QStringLiteral("传承人："), m_inheritorEdit);

    m_protectionCombo = new QComboBox;
    m_protectionCombo->addItems({
        QStringLiteral("世界级"), QStringLiteral("国家级"), QStringLiteral("省级"),
        QStringLiteral("市级"), QStringLiteral("濒危")
    });
    form->addRow(QStringLiteral("保护级别："), m_protectionCombo);

    m_digitalCombo = new QComboBox;
    m_digitalCombo->addItems({
        QStringLiteral("已数字化"), QStringLiteral("数字化中"), QStringLiteral("未数字化")
    });
    form->addRow(QStringLiteral("数字化状态："), m_digitalCombo);

    m_heatSpin = new QSpinBox;
    m_heatSpin->setRange(0, 100);
    m_heatSpin->setValue(50);
    form->addRow(QStringLiteral("国际热度指数："), m_heatSpin);

    m_imageEdit = new QLineEdit;
    m_imageEdit->setPlaceholderText(QStringLiteral("图片文件名，如 10001.jpg"));
    form->addRow(QStringLiteral("封面图片："), m_imageEdit);

    layout->addLayout(form);

    /* 按钮 */
    auto *btnLayout = new QHBoxLayout;
    auto *saveBtn = new QPushButton(QStringLiteral("保 存"));
    auto *cancelBtn = new QPushButton(QStringLiteral("取 消"));
    btnLayout->addStretch();
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    layout->addLayout(btnLayout);

    connect(saveBtn, &QPushButton::clicked, this, &ArchiveEditDialog::onSave);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    /* 编辑模式：填充现有数据 */
    if (m_editId > 0) {
        Archive *a = archive_node_find_by_id(g_ctx.archiveHead, m_editId);
        if (a) {
            m_nameEdit->setText(QString::fromUtf8(a->name));
            m_categoryCombo->setCurrentIndex((int)a->category);
            m_originEdit->setText(QString::fromUtf8(a->origin));
            m_inheritorEdit->setText(QString::fromUtf8(a->inheritor));
            m_protectionCombo->setCurrentIndex((int)a->protection);
            m_digitalCombo->setCurrentIndex((int)a->digitalStatus);
            m_heatSpin->setValue(a->heatIndex);
            m_imageEdit->setText(QString::fromUtf8(a->imagePath));
        }
    }
}

void ArchiveEditDialog::onSave()
{
    QString name = m_nameEdit->text().trimmed();
    QString origin = m_originEdit->text().trimmed();
    QString inheritor = m_inheritorEdit->text().trimmed();
    QString image = m_imageEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("项目名称不能为空"));
        return;
    }

    /* 校验管道符 */
    QByteArray nameBytes = name.toUtf8();
    QByteArray originBytes = origin.toUtf8();
    QByteArray inheritorBytes = inheritor.toUtf8();
    QByteArray imageBytes = image.toUtf8();

    if (!archive_validate_text(nameBytes.constData()) ||
        !archive_validate_text(originBytes.constData()) ||
        !archive_validate_text(inheritorBytes.constData()) ||
        !archive_validate_text(imageBytes.constData())) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("文本字段不能包含 | 字符"));
        return;
    }

    Archive data;
    memset(&data, 0, sizeof(Archive));
    strncpy(data.name, nameBytes.constData(), MAX_NAME_LEN - 1);
    data.category = (CultureCategory)m_categoryCombo->currentIndex();
    strncpy(data.origin, originBytes.constData(), MAX_ORIGIN_LEN - 1);
    strncpy(data.inheritor, inheritorBytes.constData(), MAX_NAME_LEN - 1);
    data.protection = (ProtectionLevel)m_protectionCombo->currentIndex();
    data.digitalStatus = (DigitalStatus)m_digitalCombo->currentIndex();
    data.heatIndex = m_heatSpin->value();
    strncpy(data.imagePath, imageBytes.constData(), MAX_PATH_LEN - 1);

    int ok;
    if (m_editId > 0) {
        ok = archive_update(m_editId, &data);
    } else {
        ok = archive_add(&data);
    }

    if (ok) {
        accept();
    } else {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("操作失败"));
    }
}
