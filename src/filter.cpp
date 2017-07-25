#include "filter.h"
#include <QRegExp>
#include <QDebug>

Filter::~Filter() {}

StringFilter::StringFilter(const QString &pattern) : _pattern(pattern.toUpper()) {
  _re = std::unique_ptr<QRegExp>(new QRegExp(_pattern));
}

StringFilter::~StringFilter() {}

bool StringFilter::inFilter(const QString &line) {
  return _re->indexIn(line.toUpper()) != -1;
}

FilterUnion::FilterUnion() {}

void FilterUnion::addFilter(const Filter_Ptr &f) {
  _filters.push_back(f);
}

void FilterUnion::clearFilters() {
  _filters.clear();
}

bool FilterUnion::inFilter(const QString &line) {
  for (auto &f : _filters) {
    if (!f->inFilter(line)) {
      return false;
    }
  }
  return true;
}

bool DateRangeFilter::inFilter(const QString &line) {
  QRegExp dateRegex("\\d{2}:\\d{2}:\\d{2}\\.?\\d*");
  if (dateRegex.indexIn(line) != -1) {
    auto ct = dateRegex.capturedTexts();
    for (auto &&captured_str : ct) {
      auto time = QTime::fromString(captured_str);
      if (from <= time && to >= time) {
        return true;
      }
    }
  }

  return false;
}