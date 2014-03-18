#pragma once
#include "precomp.h"

#include "common/common.h"

QDataStream &operator<<(QDataStream &out, const om::common::SegIDSet &);
QDataStream &operator>>(QDataStream &in, om::common::SegIDSet &);

QDataStream &operator>>(QDataStream &in, om::common::IDSet &);
