#ifndef LISTBOXEDITABLEITEM_H
#define LISTBOXEDITABLEITEM_H

#include <QItemDelegate>
#include <QObject>
#include <QPainter>
#include <QStyledItemDelegate>

class ListboxEditableItem : public QItemDelegate
{
public:
    explicit  ListboxEditableItem(QObject  *parent=0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
//        /painter->save();
        if(option.state & QStyle::State_Selected){
            painter->setBrush(QBrush(Qt::red));
        }else{
            painter->setBrush(QBrush(Qt::white));
        }
        painter->drawRect(option.rect);

        QItemDelegate::paint(painter, option, index);
        painter->setPen(QPen(Qt::black));
        auto value=index.data(Qt::DisplayRole);
//        if(value.isValid()){
//            auto text= value.toString();
//            painter->drawText(option.rect, Qt::AlignLeft, text);
//        }
        painter->restore();
    }
};

#endif // LISTBOXEDITABLEITEM_H
