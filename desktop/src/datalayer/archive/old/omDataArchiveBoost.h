#pragma once

#include "common/omCommon.h"

QDataStream &operator<<(QDataStream & out, const OmIDsSet & set );
QDataStream &operator>>(QDataStream & in, OmIDsSet & set );

QDataStream &operator<<(QDataStream & out, const OmSegIDsSet & set );
QDataStream &operator>>(QDataStream & in, OmSegIDsSet & set );

