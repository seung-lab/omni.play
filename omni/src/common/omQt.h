#ifndef OM_QT_H
#define OM_QT_H

#define QT_USE_FAST_CONCATENATION
#define QT_USE_FAST_OPERATOR_PLUS
#include <QString>

#include "common/omColors.h"

class QTextStream;
QTextStream &operator<<(QTextStream& out, const OmColor& c);

class QColor;
std::ostream& operator<<(std::ostream &out, const QColor& c);

#endif
