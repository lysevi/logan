#include "filter.h"
#include <QRegExp>

Filter::~Filter() {}

StringFilter::StringFilter(const QString &pattern) : _pattern(pattern.toUpper()) {
  _re = std::make_unique<QRegExp>(_pattern);
}

StringFilter::~StringFilter() {}

bool StringFilter::inFilter(const QString &line) {
  return _re->indexIn(line.toUpper()) != -1;
}
