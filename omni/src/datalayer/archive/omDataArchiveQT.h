#ifndef OM_DATA_ARCHIVE_QT_H
#define OM_DATA_ARCHIVE_QT_H

class OmDataPath;
class OmMipChunk;
class OmProject;

class OmDataArchiveQT
{
 public:
	static void ArchiveRead( const OmDataPath & path, OmMipChunk * chunk );
	static void ArchiveWrite( const OmDataPath & path, OmMipChunk * chunk );

	static void ArchiveRead( const OmDataPath & path, OmProject * project );
	static void ArchiveWrite( const OmDataPath & path, OmProject * project );
};

#endif
