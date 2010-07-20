#include "omDataArchiveQT.h"
#include "omDataArchiveProject.h"
#include "omDataArchiveMipChunk.h"
#include "omDataArchiveSegment.h"
#include "common/omDebug.h"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "volume/omMipChunk.h"

void OmDataArchiveQT::ArchiveRead( const OmDataPath & path, OmMipChunk * chunk ) 
{
	OmDataArchiveMipChunk::ArchiveRead( path, chunk );
}

void OmDataArchiveQT::ArchiveWrite( const OmDataPath & path, OmMipChunk * chunk ) 
{
	OmDataArchiveMipChunk::ArchiveWrite( path, chunk );
}

void OmDataArchiveQT::ArchiveRead( const OmDataPath & path, OmProject * project ) 
{
	OmDataArchiveProject::ArchiveRead( path, project ) ;
}

void OmDataArchiveQT::ArchiveWrite( const OmDataPath & path, OmProject * project ) 
{
	OmDataArchiveProject::ArchiveWrite( path, project );
}
