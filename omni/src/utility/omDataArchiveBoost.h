#ifndef OM_DATA_ARCHIVE_BOOST_H
#define OM_DATA_ARCHIVE_BOOST_H

#include "common/omCommon.h"

QDataStream &operator<<(QDataStream & out, const OmIds & set );
QDataStream &operator>>(QDataStream & in, OmIds & set );

#endif
