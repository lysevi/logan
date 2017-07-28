#include "filter.h"
#include <QDebug>
#include <QRegExp>

Filter::~Filter() {}

StringFilter::StringFilter(const QString &pattern) : _pattern(pattern.toUpper()) {
  _re = std::unique_ptr<QRegExp>(new QRegExp(_pattern));
}

StringFilter::~StringFilter() {}

bool StringFilter::inFilter(const QString &line) {
  return _re->indexIn(line.toUpper()) != -1;
}

FilterUnion::FilterUnion(UnionKind uk) {
  _uk = uk;
}

void FilterUnion::addFilter(const Filter_Ptr &f) {
  _filters.push_back(f);
}

void FilterUnion::clearFilters() {
  _filters.clear();
}

bool FilterUnion::inFilter(const QString &line) {
  for (auto &f : _filters) {
    auto res = f->inFilter(line);
    if (!res) {
      if (_uk == UnionKind::AND) {
        return false;
      }
    } else {
      if (_uk == UnionKind::OR) {
        return true;
      }
    }
  }
  if (_uk == UnionKind::AND) {
    return true;
  } else {
    return false;
  }
}

bool DateRangeFilter::inFilter(const QString &line) {
  if (_re->indexIn(line) != -1) {
    auto ct = _re->capturedTexts();
    for (auto &&captured_str : ct) {
      auto time = QTime::fromString(captured_str);
      if (from <= time && to >= time) {
        return true;
      }
    }
  }

  return false;
}
