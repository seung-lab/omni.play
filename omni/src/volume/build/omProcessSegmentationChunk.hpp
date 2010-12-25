#ifndef OM_PROCESS_SEGMENTATION_CHUNK_HPP
#define OM_PROCESS_SEGMENTATION_CHUNK_HPP

#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "volume/omMipChunkCoord.h"

class OmProcessSegmentationChunk {
private:
	OmMipChunk *const chunk_;
	const OmMipChunkCoord coord_;
	const bool computeSizes_;
	const Vector3i minVertexOfChunk_;
	OmSegmentCache *const segCache_;

public:
	OmProcessSegmentationChunk(OmMipChunk* chunk,
							   OmSegmentCache* segCache)
		: chunk_(chunk)
		, coord_(chunk_->GetCoordinate())
		, computeSizes_(0 == coord_.Level)
		, minVertexOfChunk_(chunk_->GetExtent().getMin())
		, segCache_(segCache)
	{
		chunk_->mDirectlyContainedValues.clear();
	}

	~OmProcessSegmentationChunk()
	{
		FOR_EACH(iter, localSegCache_){
			const OmSegID val = iter->first;
			OmSegment* seg = iter->second;
			seg->addToSize(sizes_[val]);
			seg->addToBounds(bounds_[val]);
		}

		chunk_->containedValuesDataLoaded = true;
		chunk_->WriteMetaData();
		std::cout << "chunk " << coord_
				  << " has " << chunk_->GetUniqueSegIDs().size()
				  << " values\n";
	}

	inline void processVoxel(const OmSegID val, const Vector3i& voxelPos)
	{
		chunk_->mDirectlyContainedValues.insert(val);

		if(!computeSizes_){
			return;
		}

		getOrAddSegment(val);
		sizes_[val] = 1 + sizes_[val];
		bounds_[val].merge(DataBbox(minVertexOfChunk_ + voxelPos,
									minVertexOfChunk_ + voxelPos));
	}

private:
	boost::unordered_map<OmSegID, OmSegment*> localSegCache_;
	boost::unordered_map<OmSegID, uint64_t> sizes_;
	boost::unordered_map<OmSegID, DataBbox> bounds_;

	OmSegment* getOrAddSegment(const OmSegID val)
	{
		if(!localSegCache_.count(val)){
			return localSegCache_[val] = segCache_->GetOrAddSegment(val);
		}
		return localSegCache_[val];
	}
};


#endif
