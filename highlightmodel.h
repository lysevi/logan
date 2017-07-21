#ifndef HIGHLIGHTMODEL_H
#define HIGHLIGHTMODEL_H

#include "log.h"
#include <QAbstractTableModel>
#include <QObject>
#include <QVector>
#include <QDebug>

class HighlightModel : public QAbstractTableModel {
public:
  HighlightModel(const HighlightPatterns &hp, QObject *parrent = nullptr);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole) override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override {
    Q_UNUSED(parent);
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
    return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int r = 0; r < count; ++r){
        _hp.remove(r);
    }

    endRemoveRows();

    return true;
  }

  bool insertRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override {
      Q_UNUSED(parent);
    beginResetModel();
    for (int i = 0; i < count; ++i) {
      HighlightPattern hp;
      hp.pattern = "*";
      hp.rgb = "#000000";
      _hp.insert(row, hp);
    }
    endResetModel();
    return true;
  }

  QVector<HighlightPattern> _hp;

  HighlightPatterns result();
signals:
  void editCompleted(const QString &);
};

#endif // HIGHLIGHTMODEL_H
