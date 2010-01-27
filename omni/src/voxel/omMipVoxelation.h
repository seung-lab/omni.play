#ifndef OM_MIP_VOXELATION_H
#define OM_MIP_VOXELATION_H

/*
 *
 *
 */

#include "voxel/omMipSegmentDataCoord.h"

#include "volume/omVolumeTypes.h"
#include "segment/omSegmentTypes.h"
#include "system/omCacheableBase.h"
#include "common/omStd.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;


class OmMipVoxelation;
class OmMipVoxelationManager;



class OmMipVoxelation : protected OmCacheableBase {

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
	
private:
	OmMipSegmentDataCoord mCoordinate;
	set< DataCoord > mVoxels;
};



#endif
