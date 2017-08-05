#ifndef HIGHLIGHTMODEL_H
#define HIGHLIGHTMODEL_H

#include "pattern.h"
#include <QAbstractTableModel>
#include <QDebug>
#include <QObject>
#include <QVector>

class HighlightModel : public QAbstractTableModel {
  Q_OBJECT
public:
  HighlightModel(const HighlightPatterns &hp, QObject *parrent = nullptr);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole) override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

  QVector<HighlightPattern> _hp;

  HighlightPatterns result();

  void resetTo(const HighlightPatterns &hp);
};

#endif // HIGHLIGHTMODEL_H
