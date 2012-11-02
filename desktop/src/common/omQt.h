#pragma once

#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS
#include <QString>

#include "common/omColors.h"

class QTextStream;
QTextStream &operator<<(QTextStream& out, const om::common::Color& c);

class QColor;
std::ostream& operator<<(std::ostream &out, const QColor& c);

