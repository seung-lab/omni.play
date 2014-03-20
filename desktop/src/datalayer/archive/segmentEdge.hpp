#pragma once

#include "segment/types.hpp"
#include <QDataStream>

static QDataStream& operator<<(QDataStream& out,
                               const om::segment::UserEdge& se) {
  out << se.parentID;
  out << se.childID;
  out << se.threshold;

  return out;
}

static QDataStream& operator>>(QDataStream& in, om::segment::UserEdge& se) {
  in >> se.parentID;
  in >> se.childID;
  in >> se.threshold;
  se.valid = true;

  return in;
}
