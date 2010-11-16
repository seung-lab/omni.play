#ifndef OM_SEGMENT_SELECT_ACTION_H
#define OM_SEGMENT_SELECT_ACTION_H

#include "actions/details/omAction.h"

class OmSegmentSelectActionImpl;

class OmSegmentSelectAction : public OmAction {
public:
        OmSegmentSelectAction(boost::shared_ptr<OmSegmentSelectActionImpl> impl) : impl_(impl) {}

	OmSegmentSelectAction(const OmID segmentationId,
						  const OmSegIDsSet & mNewSelectedIdSet,
						  const OmSegIDsSet & mOldSelectedIdSet,
						  const OmID segmentJustSelected,
						  void* sender,
						  const std::string & comment,
						  const bool doScroll,
						  const bool addToRecentList);
private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string &);

	boost::shared_ptr<OmSegmentSelectActionImpl> impl_;
};

#endif
