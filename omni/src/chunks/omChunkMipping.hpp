#ifndef OM_CHUNK_MIPPING_HPP
#define OM_CHUNK_MIPPING_HPP

#include "volume/omMipVolume.h"

class OmChunkMipping {
private:
	//octree properties
	DataBbox dataExtent_;
	NormBbox normExtent_;		// extent of chunk in norm space
	NormBbox clippedNormExtent_;	// extent of contained data in norm space
	OmChunkCoord coord_;
	OmChunkCoord parentCoord_;
	std::set<OmChunkCoord> childrenCoordinates_;

	uint32_t numVoxels_;
	uint32_t numBytes_;

public:
	OmChunkMipping()
	{}

	virtual ~OmChunkMipping()
	{}

	template <typename T>
	void Init(T* vol, const OmChunkCoord& coord)
	{
		//set coordinate
		coord_ = coord;

		//set parent, if any
		if (coord.Level == vol->Coords().GetRootMipLevel()) {
			parentCoord_ = OmChunkCoord::NULL_COORD;
		} else {
			parentCoord_ = coord.ParentCoord();
		}

		//set children
		vol->Coords().ValidMipChunkCoordChildren(coord, childrenCoordinates_);

		//get extent from coord
		dataExtent_ = vol->Coords().MipCoordToDataBbox(coord, coord.Level);

		//set norm extent
		normExtent_ = vol->Coords().MipCoordToNormBbox(coord);

		//set clipped norm extent
		clippedNormExtent_ = vol->Coords().MipCoordToNormBbox(coord);
		clippedNormExtent_.intersect(AxisAlignedBoundingBox<float>::UNITBOX);

		numVoxels_ = vol->Coords().GetNumberOfVoxelsPerChunk();
		numBytes_ = numVoxels_ * vol->GetBytesPerVoxel();
	}

	inline const DataBbox& GetExtent() const {
		return dataExtent_;
	}

	inline const NormBbox& GetNormExtent() const {
		return normExtent_;
	}

	inline const NormBbox& GetClippedNormExtent() const {
		return clippedNormExtent_;
	}

	inline bool IsRoot() const
	{
		//if parent is null
		return OmChunkCoord::NULL_COORD == parentCoord_;
	}

	inline const OmChunkCoord& GetParentCoordinate() const {
		return parentCoord_;
	}

	inline const std::set<OmChunkCoord>& GetChildrenCoordinates() const {
		return childrenCoordinates_;
	}

	inline uint32_t NumVoxels() const {
		return numVoxels_;
	}

	inline uint32_t NumBytes() const {
		return numBytes_;
	}
};

#endif
