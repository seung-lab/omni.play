#ifndef OM_SEG_CHUNK_H
#define OM_SEG_CHUNK_H

/**
 *	OmSegChunk extends OmChunk with extra funtionality specific to
 *   manipulating segmentation data.
 *
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/omChunk.h"
#include "chunks/omSegChunkData.h"

class OmSegmentation;

class OmSegChunk : public OmChunk {
private:
	OmSegmentation *const vol_;
	const boost::scoped_ptr<OmSegChunkData> segChunkData_;

	boost::unordered_set<OmSegID> modifiedSegIDs_;

public:
	OmSegChunk(OmSegmentation* vol, const OmChunkCoord& coord);
	virtual ~OmSegChunk();

	uint32_t GetVoxelValue(const DataCoord& vox);
	void SetVoxelValue(const DataCoord& vox, const uint32_t value);

	OmSegChunkData* SegData(){
		return segChunkData_.get();
	}
};

#endif
