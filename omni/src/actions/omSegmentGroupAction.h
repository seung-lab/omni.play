#ifndef OM_SEGMENT_GROUP_ACTION_H
#define OM_SEGMENT_GROUP_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "common/omCommon.h"

class OmSegmentGroupActionImpl;

class OmSegmentGroupAction : public OmAction {
public:
	OmSegmentGroupAction(const OmID segmentationId,
						 const OmSegIDsSet & selectedSegmentIdsSet,
						 const OmGroupName name,
						 const bool create);
private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string &);

	boost::shared_ptr<OmSegmentGroupActionImpl> impl_;
};

#endif
