#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include "log.h"
#include <QDebug>
#include <QList>
#include <QObject>

class Controller : public QObject {
  Q_OBJECT
public:
  Controller(QObject *parent = nullptr);
  Log *openFile(const QString &encoding, const QString &fname);

  HighlightPatterns m_global_highlight;

protected:
  QHash<QString, Log *> m_logs;
  QSet<QString> m_heightlight;

public slots:
  void updateAllSlot(const QString &msg);

  void closeFileSlot(const QString &fname);
  void addHighlightedTextSlot(const HighlightPattern &s);
  void clearHighlightedTextSlot();
};

#endif // WINDOWCONTROLLER_H
