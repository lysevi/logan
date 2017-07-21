#include "highlightmodel.h"

HighlightModel::HighlightModel(const HighlightPatterns &hp, QObject *parrent)
    : QAbstractTableModel(parrent) {
  int i = 0;
  _hp.resize(hp.size());
  for (auto v : hp) {
    _hp[i++] = v;
  }
}

int HighlightModel::rowCount(const QModelIndex & /*parent*/) const {
  return _hp.size();
}

int HighlightModel::columnCount(const QModelIndex & /*parent*/) const {
  return 2;
}

QVariant HighlightModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    return index.column() == 0 ? _hp[index.row()].pattern : _hp[index.row()].rgb;
  }
  return QVariant();
}

QVariant HighlightModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      switch (section) {
      case 0:
        return QString("pattern");
      case 1:
        return QString("color");
      }
    }
    return QVariant();
  }
  return QVariant();
}

bool HighlightModel::setData(const QModelIndex &index, const QVariant &value,
                             int role) {
  if (role == Qt::EditRole) {
      QString result;
    if (index.column() == 0) {
      _hp[index.row()].pattern = value.toString();
      result=_hp[index.row()].pattern;
    } else {
      _hp[index.row()].rgb = value.toString();
      result=_hp[index.row()].rgb;
    }

    emit editCompleted(result);
  }
  return true;
}

Qt::ItemFlags HighlightModel::flags(const QModelIndex & /*index*/) const {
  return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void HighlightModel::editCompleted(const QString &){

}

HighlightPatterns HighlightModel::result(){
    HighlightPatterns result;
    for(auto&v:_hp){
        result[v.pattern]=v;
    }
    return result;
}