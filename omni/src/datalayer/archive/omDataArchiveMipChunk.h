#ifndef OM_DATA_ARCHIVE_MIP_CHUNK_H
#define OM_DATA_ARCHIVE_MIP_CHUNK_H

class OmDataPath;
class OmMipChunk;
class OmSimpleChunk;

class OmDataArchiveMipChunk
{
 public:
	static void ArchiveRead( const OmDataPath & path, OmMipChunk * chunk );
	static void ArchiveWrite( const OmDataPath & path, OmMipChunk * chunk );
	static void ArchiveRead( const OmDataPath & path, OmSimpleChunk * chunk );
	static void ArchiveWrite( const OmDataPath & path, OmSimpleChunk * chunk );
};

QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk );
QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk );
QDataStream &operator<<(QDataStream & out, const OmSimpleChunk & chunk );
QDataStream &operator>>(QDataStream & in, OmSimpleChunk & chunk );

#endif
