#ifndef LOG_H
#define LOG_H

#include "filter.h"
#include "logviewer.h"
#include "pattern.h"
#include <QAbstractItemModel>
#include <QDateTime>
#include <QFileInfo>
#include <QListView>
#include <QSet>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

struct LinePosition {
  int first;
  int second;
};

using LinePositionList = std::vector<LinePosition>;

struct CachedString {
  int index; // original string index in file.
  std::shared_ptr<QString> originValue;
  std::shared_ptr<QString> Value;

  void clear() { originValue = Value = nullptr; }
};

enum class SearchDirection { Up, Down };

class Log : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(QString name READ name NOTIFY nameChanged)
  Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
public:
  Log(const QFileInfo &fileInfo, const QString &filename,
      const HighlightPatterns *global_highlight, const QString &default_encoding,
      QObject *parent = nullptr);

  static Log *openFile(const QString &fname, const HighlightPatterns *global_highlight,
                       const QString &defaul_encoding, QObject *parent = nullptr);

  QString name() const;
  QString filename() const;

  void update();

  //// QAbstractItemModel methods
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  bool setData(const QModelIndex &index, const QVariant &value, int role) {
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return true;
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    Q_UNUSED(parent);
    return 1;
  }

  Qt::ItemFlags flags(const QModelIndex &index) const {
    Q_UNUSED(index)
    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }

  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override {
    if (!hasIndex(row, column, parent)) {
      return QModelIndex();
    }

    return createIndex(row, column);
  }

  QModelIndex parent(const QModelIndex & /*child*/) const override {
    return QModelIndex();
  }

  //// Log methods
  static bool heighlightStr(QString *str, const HighlightPattern &pattern);

  void setListVoxObject(LogViewer *object);

  QString plainText(const QModelIndex &index) const;

  QPair<int, QString> findFrom(const QString &pattern, int index,
                               SearchDirection direction);

  void setFilter(QProgressDialog *progress_dlg, const Filter_Ptr &fltr);
  void resetFilter(QProgressDialog *progress_dlg, const Filter_Ptr &fltr);
  void clearFilter();

  QDateTime lastUpdate() const { return m_fileInfo.lastModified(); }

  size_t linesCount() const { return m_lines.size(); }
signals:
  void linesChanged();
  void countChanged(int);
  void nameChanged();
  void filenameChanged();
public slots:
protected:
  void loadFile();
  std::shared_ptr<QString> makeRawString(int row) const;
  void rawStringToValue(std::shared_ptr<QString> &rawString) const;
  std::shared_ptr<QString> makeString(int row) const;

  void setFilter_impl(QProgressDialog *progress_dlg, const Filter_Ptr &fltr);

protected:
  mutable std::mutex _locker;
  bool m_load_complete = false;
  QString m_name;
  QString m_fname;

  LinePositionList m_lines;
  QByteArray m_bts;
  mutable QVector<CachedString> m_cache;
  const HighlightPatterns *m_global_highlight;

  QTextCodec *m_codec;
  LogViewer *m_lv_object;
  QFileInfo m_fileInfo;
  QDateTime m_lastModifed;
  QString m_default_encoding;

  Filter_Ptr _fltr;
  mutable QVector<CachedString> m_fltr_cache;
};

#endif // LOG_H
