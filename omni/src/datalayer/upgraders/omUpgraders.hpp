#ifndef OM_UPGRADERS_HPP
#define OM_UPGRADERS_HPP

#include "datalayer/upgraders/omUpgradeTo14.hpp"
#include "datalayer/upgraders/omUpgradeTo20.hpp"

class OmUpgraders{
public:
	static void RebuildCenterOfSegmentData()
	{
		FOR_EACH(iter, OmProject::GetValidSegmentationIds()){
			const SegmentationDataWrapper sdw(*iter);
			OmSegmentUtils::RebuildCenterOfSegmentData(sdw);
		}
	}

	static void to14(){
		OmUpgradeTo14 u;
		u.copyDataOutFromHDF5();
	}
	static void to20(){
		OmUpgradeTo20 u;
		u.GroupUpValidSegments();
	}
};

#endif
