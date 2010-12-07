#ifndef OM_UPGRADE_TO_20_HPP
#define OM_UPGRADE_TO_20_HPP

#include "segment/details/omSegmentListContainer.hpp"
#include "segment/lowLevel/omSegmentListBySize.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentLists.hpp"
#include "segment/omSegmentUtils.hpp"

class OmUpgradeTo20{
public:
	void GroupUpValidSegments(){
		SegmentationDataWrapper sdw(1);
		groupUpValidSegments(sdw);
	}

private:
	void groupUpValidSegments(const SegmentationDataWrapper& sdw)
	{
		printf("rewriting valid segment data...\n");

		boost::shared_ptr<OmSegmentLists> segLists = sdw.GetSegmentLists();
		OmSegmentListContainer<OmSegmentListBySize>& validListCont
			= segLists->Valid();
		OmSegmentListBySize& validList = validListCont.List();

		boost::shared_ptr<OmValidGroupNum>& validGroupNum
			= sdw.GetValidGroupNum();

		OmSegIDsSet allSegIDs = validList.AllSegIDs();
		FOR_EACH(iter, allSegIDs){
			const OmSegID segID = *iter;
			OmSegIDsSet set;
			set.insert(segID);

			boost::shared_ptr<std::set<OmSegment*> > children =
				OmSegmentUtils::GetAllChildrenSegments(sdw.GetSegmentCache(), set);

			validGroupNum->Set(sdw, children, true);
		}
	}
};

#endif
