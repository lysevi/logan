#pragma once

#include "common_re.h"
#include <QString>
#include <QTime>
#include <list>
#include <memory>

struct Filter {
  virtual bool inFilter(const QString &line) = 0;
  virtual size_t filters() const { return size_t(1); }
  virtual ~Filter();
};

using Filter_Ptr = std::shared_ptr<Filter>;

class QRegExp;

struct StringFilter : public Filter {
  StringFilter(const QString &pattern);
  ~StringFilter();

  bool inFilter(const QString &line) override;

  std::unique_ptr<QRegExp> _re;
  QString _pattern;
};

struct FilterUnion : public Filter {
  FilterUnion();

  void addFilter(const Filter_Ptr &f);
  void clearFilters();

  bool inFilter(const QString &line) override;
  size_t filters() const override { return _filters.size(); }
  std::list<Filter_Ptr> _filters;
};

struct DateRangeFilter : public Filter {
  DateRangeFilter(QTime _from, QTime _to) {
    from = _from;
    to = _to;
    _re = std::unique_ptr<QRegExp>(new QRegExp(common_re::dateRe));
  }

  bool inFilter(const QString &line) override;
  QTime from, to;
  std::unique_ptr<QRegExp> _re;
};
