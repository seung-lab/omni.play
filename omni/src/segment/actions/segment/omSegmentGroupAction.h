#ifndef OM_SEGMENT_GROUP_ACTION_H
#define OM_SEGMENT_GROUP_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "common/omCommon.h"

class OmSegmentGroupAction : public OmAction {

public:
	OmSegmentGroupAction( const OmId segmentationId, const OmSegIDsSet & selectedSegmentIdsSet, const OmGroupName name, const bool create);

private:
	void Action();
	void UndoAction();
	string Description();

	const OmId mSegmentationId;
	const OmGroupName mName;
	const bool mCreate;
	OmSegIDsSet mSelectedSegmentIds;
};

#endif
