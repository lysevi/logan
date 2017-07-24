#include "listboxeditableitem.h"

ListboxEditableItem::ListboxEditableItem(QObject *parent)
    : QAbstractItemDelegate(parent) {}

void ListboxEditableItem::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {

  painter->save();

  // painter->setFont(m_default_font);

  auto value = index.model()->data(index, Qt::DisplayRole).toString();

  QTextDocument doc;
  QPalette::ColorGroup cg =
      option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
  if (cg == QPalette::Normal && !(option.state & QStyle::State_Active)) {
    cg = QPalette::Inactive;
  }

  // doc.setTextWidth(rect.width());
  doc.setHtml(value);
  doc.setDocumentMargin(1);
  doc.setDefaultFont(m_default_font);

  // qDebug()<<"font:"<<m_default_font.toString()<<" => "<<doc.defaultFont().toString();
  option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

  painter->translate(option.rect.left(), option.rect.top());

  QRect clip(0, 0, option.rect.width(), option.rect.height());

  doc.drawContents(painter, clip);

  painter->restore();
}
