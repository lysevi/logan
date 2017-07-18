#ifndef LISTBOXEDITABLEITEM_H
#define LISTBOXEDITABLEITEM_H

#include <QItemDelegate>
#include <QObject>
#include <QTextEdit>
#include <QStyleOption>
#include <QPainter>

class ListboxEditableItem : public QItemDelegate
{
public:
    explicit  ListboxEditableItem(QObject  *parent=0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override{


        painter->save();

        auto value = index.model()->data(index, Qt::DisplayRole).toString();
        QTextDocument doc;
        doc.setHtml(value);


        option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

        painter->translate(option.rect.left(), option.rect.top());
        QRect clip(0, 0, option.rect.width(), option.rect.height());
        doc.drawContents(painter, clip);

        painter->restore();
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override{
        Q_UNUSED(index);
        Q_UNUSED(option);
        QTextEdit *result=new QTextEdit(parent);
        //result->setText(index.data());
        return result;
    }


    void setEditorData(QWidget *editor, const QModelIndex &index) const override{
        auto value = index.model()->data(index, Qt::EditRole).toString();
        static_cast<QTextEdit*>(editor)->setText(value);
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
};

#endif // LISTBOXEDITABLEITEM_H
