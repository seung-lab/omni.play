#ifndef OM_MIP_VOXELATION_MANAGER_H
#define OM_MIP_VOXELATION_MANAGER_H

/*
 *
 *
 */


#include "voxel/omMipVoxelation.h"
#include "system/omThreadedCache.h"
#include "common/omStd.h"


class OmMipSegmentDataCoord;
class OmSegmentManager;
class OmMipVolume;


typedef OmThreadedCache< OmMipSegmentDataCoord, OmMipVoxelation > MipVoxelationCache;



class OmMipVoxelationManager : public MipVoxelationCache {

public:
	OmMipVoxelationManager(OmMipVolume *);
	~OmMipVoxelationManager();
	
	//accessors
	shared_ptr<OmMipVoxelation> GetVoxelation(const OmMipSegmentDataCoord & );
	bool ContainsVoxelation(const OmMipSegmentDataCoord & );
	void RemoveVoxelation(const OmMipSegmentDataCoord & );
	
	//generation
	bool IsBoundaryVoxel(const DataCoord &);
	OmMipVoxelation* CreateMipVoxelation(const OmMipSegmentDataCoord &);
	void RefreshNeighboringVoxels(const DataCoord &rDataCoord, OmMipVoxelation &rVoxelation);
	void UpdateVoxel(const DataCoord &rVox, const SEGMENT_DATA_TYPE old_val, const SEGMENT_DATA_TYPE new_val );
	
	//draw
	void DrawVoxelations(OmSegmentManager &rSegMgr, const OmMipChunkCoord &mipCoord, const SegmentDataSet &rRelvDataVals, const OmBitfield &drawOps);
	
private:
	OmMipVoxelation* HandleCacheMiss(const OmMipSegmentDataCoord &);
	void HandleFetchUpdate();

	OmMipVolume * const mpMipVolume;
};









#endif
