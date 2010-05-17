#include "omDataArchiveQT.h"
#include "omDataArchiveProject.h"
#include "omDataArchiveMipChunk.h"
#include "omDataArchiveSegment.h"

void OmDataArchiveQT::ArchiveRead( OmHdf5Path path, OmMipChunk * chunk ) 
{
	OmDataArchiveMipChunk::ArchiveRead( path, chunk );
}

void OmDataArchiveQT::ArchiveWrite( OmHdf5Path path, OmMipChunk * chunk ) 
{
	OmDataArchiveMipChunk::ArchiveWrite( path, chunk );
}

void OmDataArchiveQT::ArchiveRead( OmHdf5Path path, OmSimpleChunk * chunk ) 
{
	OmDataArchiveMipChunk::ArchiveRead( path, chunk );
}

void OmDataArchiveQT::ArchiveWrite( OmHdf5Path path, OmSimpleChunk * chunk ) 
{
	OmDataArchiveMipChunk::ArchiveWrite( path, chunk );
}

void OmDataArchiveQT::ArchiveRead( OmHdf5Path path, OmProject * project ) 
{
	OmDataArchiveProject::ArchiveRead( path, project ) ;
}

void OmDataArchiveQT::ArchiveWrite( OmHdf5Path path, OmProject * project ) 
{
	OmDataArchiveProject::ArchiveWrite( path, project );
}
