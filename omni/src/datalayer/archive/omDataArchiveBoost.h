#ifndef OM_DATA_ARCHIVE_BOOST_H
#define OM_DATA_ARCHIVE_BOOST_H

#include "common/omCommon.h"

QDataStream &operator<<(QDataStream & out, const OmIDsSet & set );
QDataStream &operator>>(QDataStream & in, OmIDsSet & set );
//QDataStream &operator<<(QDataStream & out, const OmSegIDsIntMap & map );
//QDataStream &operator>>(QDataStream & in, OmSegIDsIntMap & map );


#endif
