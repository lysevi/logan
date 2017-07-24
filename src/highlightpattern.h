#pragma once

#include <QMap>
#include <QString>

struct HighlightPattern {
  QString pattern;
  QString rgb;
};

using HighlightPatterns = QMap<QString, HighlightPattern>;

const QString dateRe = "\\d{2}:\\d{2}:\\d{2}\\.?\\d*";
const QString errorRe = "(ERR.*)";
const QString exceptRe = "(EXCEPT.*)";

const HighlightPatterns default_highlight_settings{{dateRe, {dateRe, "#0000FF"}},
                                                   {errorRe, {errorRe, "#FF0000"}},
                                                   {exceptRe, {exceptRe, "#FF0040"}}};
