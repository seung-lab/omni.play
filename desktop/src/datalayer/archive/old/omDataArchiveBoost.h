#pragma once

#include "common/common.h"

QDataStream &operator<<(QDataStream & out, const om::common::IDSet & set );
QDataStream &operator>>(QDataStream & in, om::common::IDSet & set );

QDataStream &operator<<(QDataStream & out, const om::common::SegIDSet & set );
QDataStream &operator>>(QDataStream & in, om::common::SegIDSet & set );

