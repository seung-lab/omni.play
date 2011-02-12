#include "volume/omMipVolume.h"
#include "volume/io/omVolumeData.h"

OmMipVolume::OmMipVolume()
	: mVolDataType(OmVolDataType::UNKNOWN)
	, mBuildState(MIPVOL_UNBUILT)
{}

bool OmMipVolume::ContainsVoxel(const DataCoord & vox){
	return coords_.GetDataExtent().contains(vox);
}

boost::shared_ptr<std::deque<OmChunkCoord> >
OmMipVolume::GetMipChunkCoords() const
{
	std::deque<OmChunkCoord>* coords = new std::deque<OmChunkCoord>();

	for(int level = 0; level <= coords_.GetRootMipLevel(); ++level) {
		coords_.addChunkCoordsForLevel(level, coords);
	}

	return boost::shared_ptr<std::deque<OmChunkCoord> >(coords);
}

boost::shared_ptr<std::deque<OmChunkCoord> >
OmMipVolume::GetMipChunkCoords(const int mipLevel) const
{
	std::deque<OmChunkCoord>* coords = new std::deque<OmChunkCoord>();

	coords_.addChunkCoordsForLevel(mipLevel, coords);

	return boost::shared_ptr<std::deque<OmChunkCoord> >(coords);
}
