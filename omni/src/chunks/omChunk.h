#ifndef OM_CHUNK_H
#define OM_CHUNK_H

/**
 *	OmChunk encapsulates a 3D-matrix of image data (typically 128^3) and
 *   its associated size/positioning metadata.
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/omChunkCoord.h"
#include "chunks/omChunkData.h"
#include "chunks/omChunkMipping.hpp"
#include "chunks/details/omPtrToChunkDataMemMapVol.h"
#include "volume/omVolumeTypes.hpp"

class OmChunk {
public:
	template <typename VOL>
	OmChunk(VOL* vol, const OmChunkCoord& coord)
		: coord_(coord)
		, chunkData_(new OmChunkData(vol, this))
		, ptrToChunkData_(new OmPtrToChunkDataMemMapVol(vol, coord))
	{
		mipping_.Init(vol, coord);
	}

	virtual ~OmChunk();

	bool ContainsVoxel(const DataCoord& vox) const {
		return mipping_.GetExtent().contains(vox);
	}

public:
	const OmChunkCoord& GetCoordinate() const {
		return coord_;
	}
	int GetLevel() const {
		return coord_.Level;
	}
	const Vector3i GetDimensions() const {
		return mipping_.GetExtent().getUnitDimensions();
	}

	OmChunkData* Data(){
		return chunkData_.get();
	}
	OmPtrToChunkDataBase* DataPtr(){
		return ptrToChunkData_.get();
	}
	OmChunkMipping& Mipping(){
		return mipping_;
	}

protected:
	const OmChunkCoord coord_;
	const boost::scoped_ptr<OmChunkData> chunkData_;
	const boost::scoped_ptr<OmPtrToChunkDataBase> ptrToChunkData_;

	OmChunkMipping mipping_;
};

#endif
