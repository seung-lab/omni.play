#ifndef OM_DATA_ARCHIVE_QT_H
#define OM_DATA_ARCHIVE_QT_H

class OmHdf5Path;
class OmMipChunk;
class OmSimpleChunk;
class OmProject;

class OmDataArchiveQT
{
 public:
	static void ArchiveRead( const OmHdf5Path & path, OmMipChunk * chunk );
	static void ArchiveWrite( const OmHdf5Path & path, OmMipChunk * chunk );

	static void ArchiveRead( const OmHdf5Path & path, OmSimpleChunk * chunk );
	static void ArchiveWrite( const OmHdf5Path & path, OmSimpleChunk * chunk );

	static void ArchiveRead( const OmHdf5Path & path, OmProject * project );
	static void ArchiveWrite( const OmHdf5Path & path, OmProject * project );
};

#endif
