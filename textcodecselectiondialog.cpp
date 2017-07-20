#include "textcodecselectiondialog.h"
#include "ui_textcodecselectiondialog.h"
#include <QTextCodec>
#include <QDebug>


TextCodecSelectionDialog::TextCodecSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextCodecSelectionDialog)
{
    ui->setupUi(this);
    QStringList sl;
    auto codecs=QTextCodec::availableCodecs();
    int selectedIndex=0;
    int i=0;
    for(auto&c:codecs)
    {
        auto s=QString(c);
        qDebug()<<s;
        sl<<s;

        if(s.toUpper()==defaultTextEncoding)
        {
            selectedIndex=i;
        }
        i++;
    }
    m_model.setStringList(sl);
    this->ui->listView->setModel(&m_model);
    this->ui->listView->selectionModel()->setCurrentIndex(m_model.index(selectedIndex,0),QItemSelectionModel::SelectCurrent);
}

TextCodecSelectionDialog::~TextCodecSelectionDialog()
{
    delete ui;
}

QString TextCodecSelectionDialog::selectedEncoding(){
    auto selected=this->ui->listView->selectionModel()->selectedRows();
    if(selected.empty()){
        return defaultTextEncoding;
    }else{
        auto result=m_model.data(selected.front());
        return result.toString();
    }
}
