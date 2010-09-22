#include "volume/omThreadChunkLevel.h"
#include "volume/omMipVolume.h"

OmThreadChunkLevel::OmThreadChunkLevel(const OmMipChunkCoord & rMipCoord, OmMipVolume * pMipVolume)
	:OmMipChunk(rMipCoord,pMipVolume)
{
	//init chunk properties
	InitChunk(rMipCoord);
}

OmThreadChunkLevel::~OmThreadChunkLevel()
{
	//since parent destructor is called after this child destructor, we need to call
	//child Close() here, or else child Close() won't be called (since child won't exist)
	//when called in parent destructor
	if (IsOpen()) {
		Close();
	}
}

/*
 *	Initialize chunk level with properties of given coordinate.
 */
void OmThreadChunkLevel::InitChunk(const OmMipChunkCoord & rMipCoord)
{
	//set coordinate
	mCoordinate = rMipCoord;

	//set parent, if any
	if (rMipCoord.Level == mpMipVolume->GetRootMipLevel()) {
		mParentCoord = OmMipChunkCoord::NULL_COORD;
	} else {
		mParentCoord = rMipCoord;
		mParentCoord.Level++;
	}

	//get extent from coord
	mDataExtent = mpMipVolume->MipCoordToThreadLevelDataBbox(rMipCoord);

	//set if mipvolume uses metadata
	setMetaDataClean();
}
