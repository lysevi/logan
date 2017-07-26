#pragma once

#include <QString>

namespace common_re {
const QString dateRe = "\\d{2}:\\d{2}:\\d{2}\\.?\\d*";
const QString errorRe = "(ERR.*)";
const QString exceptRe = "(EXCEPT.*)";
}
