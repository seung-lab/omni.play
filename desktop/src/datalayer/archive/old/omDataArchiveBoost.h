#pragma once

#include "common/common.h"
#include <QDataStream>

QDataStream &operator<<(QDataStream &out, const om::common::SegIDSet &);
QDataStream &operator>>(QDataStream &in, om::common::SegIDSet &);

QDataStream &operator>>(QDataStream &in, om::common::IDSet &);
