#ifndef TEXTCODECSELECTIONDIALOG_H
#define TEXTCODECSELECTIONDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QStringListModel>

namespace Ui {
class TextCodecSelectionDialog;
}

#if WIN32
const QString defaultTextEncoding="CP1251";

#else
const QString defaultTextEncoding="UTF-8";

#endif

class TextCodecSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextCodecSelectionDialog(QWidget *parent = 0);
    ~TextCodecSelectionDialog();
    QString selectedEncoding();
private:
    QStringListModel m_model;
private:
    Ui::TextCodecSelectionDialog *ui;
};

#endif // TEXTCODECSELECTIONDIALOG_H
