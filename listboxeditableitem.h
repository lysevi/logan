#pragma once

#include <QStyledItemDelegate>
#include <QObject>
#include <QTextEdit>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include "log.h"

class ListboxEditableItem : public QStyledItemDelegate
{
public:
    explicit  ListboxEditableItem(QObject  *parent=0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override{


        painter->save();

        //painter->setFont(m_default_font);

        auto value = index.model()->data(index, Qt::DisplayRole).toString();
        QTextDocument doc;
        doc.setHtml(value);
        doc.setDefaultFont(m_default_font);
        //qDebug()<<"font:"<<m_default_font.toString()<<" => "<<doc.defaultFont().toString();
        option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

        painter->translate(option.rect.left(), option.rect.top());

        QRect clip(0, 0, option.rect.width(), option.rect.height());

        doc.drawContents(painter, clip);

        painter->restore();
    }

//    QSize sizeHint(const QStyleOptionViewItem &option,
//                   const QModelIndex &index) const override{
//        auto value = index.model()->data(index, Qt::DisplayRole).toString();
//        QTextDocument doc;
//        doc.setHtml(value);
//        doc.size();
//    }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override{
        Q_UNUSED(index);
        Q_UNUSED(option);
        QTextEdit *result=new QTextEdit(parent);
        result->setFont(m_default_font);
        //result->setText(index.data());
        return result;
    }


    void setEditorData(QWidget *editor, const QModelIndex &index) const override{
        auto log=dynamic_cast<const Log*>(index.model());
        auto value =  log->plainText(index);
        static_cast<QTextEdit*>(editor)->setPlainText(value);
    }
    //    void setModelData(QWidget *editor, QAbstractItemModel *model,
    //                      const QModelIndex &index) const{
    //            //static_cast<QTextEdit*>(editor)->
    //    }

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        Q_UNUSED(index);
        editor->setGeometry(option.rect);
    }

    QFont m_default_font;
};

