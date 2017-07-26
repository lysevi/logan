#pragma once

#include "common_re.h"
#include <QMap>
#include <QString>

struct HighlightPattern {
  QString pattern;
  QString rgb;
};

using HighlightPatterns = QMap<QString, HighlightPattern>;

const HighlightPatterns default_highlight_settings{
    {common_re::dateRe, {common_re::dateRe, "#0000FF"}},
    {common_re::errorRe, {common_re::errorRe, "#FF0000"}},
    {common_re::exceptRe, {common_re::exceptRe, "#FF0040"}}};
