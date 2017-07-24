#pragma once

#include <QString>
#include <memory>

struct Filter {
  virtual bool inFilter(const QString &line) = 0;
  virtual ~Filter();
};

using Filter_Ptr=std::shared_ptr<Filter>;

class QRegExp;

struct StringFilter : public Filter{
  StringFilter(const QString&pattern);
  ~StringFilter();

  bool inFilter(const QString&line)override;

  std::unique_ptr<QRegExp> _re;
  QString _pattern;
};
