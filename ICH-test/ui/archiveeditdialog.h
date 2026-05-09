#ifndef ARCHIVEEDITDIALOG_H
#define ARCHIVEEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>

class ArchiveEditDialog : public QDialog
{
    Q_OBJECT
public:
    /* editId > 0 表示编辑模式，0 表示新增 */
    explicit ArchiveEditDialog(int editId = 0, QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    int m_editId;
    QLineEdit *m_nameEdit;
    QComboBox *m_categoryCombo;
    QLineEdit *m_originEdit;
    QLineEdit *m_inheritorEdit;
    QComboBox *m_protectionCombo;
    QComboBox *m_digitalCombo;
    QSpinBox  *m_heatSpin;
    QLineEdit *m_imageEdit;
};

#endif // ARCHIVEEDITDIALOG_H
