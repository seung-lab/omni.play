#ifndef OM_MIP_VOXELATION_MANAGER_H
#define OM_MIP_VOXELATION_MANAGER_H

#include "voxel/omMipVoxelation.h"
#include "system/omThreadedCache.h"
#include "common/omStd.h"

class OmMipSegmentDataCoord;
class OmSegmentCache;
class OmMipVolume;

typedef OmThreadedCache< OmMipSegmentDataCoord, OmMipVoxelation > MipVoxelationCache;

class OmMipVoxelationManager : public MipVoxelationCache {

public:
	OmMipVoxelationManager(OmMipVolume *);
	~OmMipVoxelationManager();
	
	//accessors
	void GetVoxelation(QExplicitlySharedDataPointer<OmMipVoxelation> &p_value, const OmMipSegmentDataCoord & );
	bool ContainsVoxelation(const OmMipSegmentDataCoord & );
	void RemoveVoxelation(const OmMipSegmentDataCoord & );
	
	//generation
	bool IsBoundaryVoxel(const DataCoord &);
	OmMipVoxelation* CreateMipVoxelation(const OmMipSegmentDataCoord &);
	void RefreshNeighboringVoxels(const DataCoord &rDataCoord, OmMipVoxelation &rVoxelation);
	void UpdateVoxel(const DataCoord &rVox, const OmSegID old_val, const OmSegID new_val );
	
	//draw
	void DrawVoxelations(OmSegmentCache *rSegMgr, const OmMipChunkCoord &mipCoord, const OmSegIDs &rRelvDataVals, const OmBitfield &drawOps);
	
private:
	OmMipVoxelation* HandleCacheMiss(const OmMipSegmentDataCoord &);
	void HandleFetchUpdate();
	OmMipVolume * const mpMipVolume;

};

#endif
