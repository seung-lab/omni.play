#ifndef OM_UPGRADE_TO_15_HPP
#define OM_UPGRADE_TO_15_HPP

#include "utility/dataWrappers.h"

class OmUpgradeTo15{
public:
	void rewriteSegments(){
		doConvert();
	}

private:

	void doConvert()
	{
		printf("upgrading segments to ver 15\n");
		FOR_EACH(iter, OmProject::GetValidSegmentationIds()){
			SegmentationDataWrapper sdw(*iter);
			OmSegmentCache* cache = sdw.getSegmentCache();
			cache->UpgradeSegmentSerialization();
		}
	}
};

#endif
