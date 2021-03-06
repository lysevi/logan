#include "controller.h"
#include <QDateTime>

Controller::Controller(QObject *parent) : QObject(parent) {
}

void Controller::update() {
  for (auto &v : m_logs) {
    v->update();
  }
}

void Controller::update(const QString &msg) {
  for (auto &v : m_logs) {
    if (msg == v->filename()) {
      v->update();
      break;
    }
  }
}

Log *Controller::openFile(const QString &encoding, const QString &fname) {
  if (m_logs.contains(fname)) {
    return nullptr;
  }
  auto log = Log::openFile(fname, &m_global_highlight, encoding, this);
  if (log == nullptr) {
    return nullptr;
  }
  m_logs[fname] = log;
  return log;
}

void Controller::closeFileSlot(const QString &fname) {
  auto it = m_logs.find(fname);

  if ((*it)->filename() == fname) {
    auto ptr = *it;
    qDebug() << "erase " << fname;
    delete ptr;
    m_logs.erase(it);
  } else {
    throw std::logic_error("fname not found");
  }
}

void Controller::addHighlightedTextSlot(const HighlightPattern &s) {
  qDebug() << "addHighlightedTextSlot " << s.pattern;
  auto curDT = QDateTime::currentDateTimeUtc();
  m_global_highlight[s.pattern] = s;
  qDebug() << "elapsed time:" << curDT.secsTo(QDateTime::currentDateTimeUtc());
}
