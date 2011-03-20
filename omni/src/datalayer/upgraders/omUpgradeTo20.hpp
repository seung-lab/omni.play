#ifndef OM_UPGRADE_TO_20_HPP
#define OM_UPGRADE_TO_20_HPP

#include "segment/details/omSegmentListContainer.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lowLevel/omSegmentListBySize.h"
#include "segment/omSegmentLists.hpp"
#include "segment/omSegmentUtils.hpp"
#include "utility/dataWrappers.h"

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

		OmSegmentListContainer<OmSegmentListBySize>& validListCont
			= sdw.SegmentLists()->Valid();
		OmSegmentListBySize& validList = validListCont.List();

		OmValidGroupNum* validGroupNum = sdw.ValidGroupNum();

		OmSegIDsSet allSegIDs = validList.AllSegIDs();
		FOR_EACH(iter, allSegIDs){
			const OmSegID segID = *iter;
			OmSegIDsSet set;
			set.insert(segID);

			boost::shared_ptr<std::set<OmSegment*> > children =
				OmSegmentUtils::GetAllChildrenSegments(sdw.SegmentCache(), set);

			validGroupNum->Set(sdw, children, true);
		}
	}
};

#endif
