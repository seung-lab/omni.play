#ifndef OM_DATA_ARCHIVE_COORDS_H
#define OM_DATA_ARCHIVE_COORDS_H

class OmMipChunkCoord;
class OmMipMeshCoord;
class OmTileCoord;

QDataStream &operator<<(QDataStream & out, const OmMipMeshCoord & c );
QDataStream &operator>>(QDataStream & in, OmMipMeshCoord & c );

QDataStream &operator<<(QDataStream & out, const OmMipChunkCoord & c );
QDataStream &operator>>(QDataStream & in, OmMipChunkCoord & c );

#endif
