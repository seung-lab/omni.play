#ifndef OM_DATA_ARCHIVE_COORDS_H
#define OM_DATA_ARCHIVE_COORDS_H

#include "common/omDebug.h"
#include "project/omProject.h"
#include "system/omProjectData.h"

class OmTileCoord;

QDataStream &operator<<(QDataStream & out, const OmMipSegmentDataCoord & c );
QDataStream &operator>>(QDataStream & in, OmMipSegmentDataCoord & c );

QDataStream &operator<<(QDataStream & out, const OmMipMeshCoord & c );
QDataStream &operator>>(QDataStream & in, OmMipMeshCoord & c );

QDataStream &operator<<(QDataStream & out, const OmMipChunkCoord & c );
QDataStream &operator>>(QDataStream & in, OmMipChunkCoord & c );

QDataStream &operator<<(QDataStream & out, const OmTileCoord & c );
QDataStream &operator>>(QDataStream & in, OmTileCoord & c );

#endif
