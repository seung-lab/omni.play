#ifndef OM_MIP_VOXELATION_H
#define OM_MIP_VOXELATION_H

#include "voxel/omMipSegmentDataCoord.h"

#include "system/omCacheableBase.h"
#include "common/omStd.h"

class OmMipVoxelation;
class OmMipVoxelationManager;

class OmMipVoxelation : public OmCacheableBase {

public:
	OmMipVoxelation(const OmMipSegmentDataCoord &mipSegDataCoord, OmMipVoxelationManager *cache);
	~OmMipVoxelation();
	
	//accessors
	SEGMENT_DATA_TYPE GetSegmentDataValue();
	
	//voxel
	void AddVoxel(DataCoord &);
	void RemoveVoxel(DataCoord &);

	//draw
	void Draw();
	
	void Flush();

private:
	OmMipSegmentDataCoord mCoordinate;
	set< DataCoord > mVoxels;
};



#endif
