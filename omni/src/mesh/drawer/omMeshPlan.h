#ifndef OM_MESH_PLAN_H
#define OM_MESH_PLAN_H

#include "segment/omSegment.h"
#include "volume/omMipChunkCoord.h"

struct LargestSegmentFirst {
	bool operator() (const OmSegment* a, const OmSegment* b) const {
		return a->size() > b->size();
	}
};

typedef std::multimap<OmSegment*,
					  OmMipChunkCoord,
					  LargestSegmentFirst> OmMeshPlanStruct;

class OmMeshPlan : public OmMeshPlanStruct {
private:
	uint64_t voxelCount_;

public:
	OmMeshPlan()
		: voxelCount_(0)
	{}

	void Add(OmSegment* seg, const OmMipChunkCoord& coord)
	{
		insert(std::make_pair(seg, coord));
		voxelCount_ += seg->size();
	}

	uint64_t TotalVoxels() const {
		return voxelCount_;
	}
};

#endif
