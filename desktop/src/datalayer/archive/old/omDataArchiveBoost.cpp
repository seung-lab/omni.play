#include "datalayer/archive/old/omDataArchiveBoost.h"

#include "zi/omUtility.h"
#include <QDataStream>

QDataStream &operator<<(QDataStream &out, const om::common::IDSet &set) {
  out << (quint32) set.size();

  om::common::IDSet::const_iterator iter;
  for (iter = set.begin(); iter != set.end(); ++iter) {
    out << *iter;
  }

  return out;
}

QDataStream &operator>>(QDataStream &in, om::common::IDSet &set) {
  quint32 size;
  in >> size;

  om::common::ID id;
  for (quint32 i = 0; i < size; ++i) {
    in >> id;
    set.insert(id);
  }

  return in;
}

QDataStream &operator<<(QDataStream &out, const om::common::SegIDSet &set) {
  out << (quint32) set.size();

  FOR_EACH(iter, set) { out << *iter; }

  return out;
}

QDataStream &operator>>(QDataStream &in, om::common::SegIDSet &set) {
  quint32 size;
  in >> size;

  om::common::ID id;
  for (quint32 i = 0; i < size; ++i) {
    in >> id;
    set.insert(id);
  }

  return in;
}
