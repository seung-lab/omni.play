#include "common/omQt.h"

#include <QColor>
#include <QTextStream>

QTextStream& operator<<(QTextStream& out, const OmColor& c) {
  out << c.red << "\t";
  out << c.green << "\t";
  out << c.blue;
  return out;
}

std::ostream& operator<<(std::ostream& out, const QColor& c) {
  // out << c.name().toStdString();
  out << "[r" << c.red() << "g," << c.green() << "b," << c.blue() << "]";
  return out;
}
