#ifndef OM_SEGMENT_JOIN_ACTION_H
#define OM_SEGMENT_JOIN_ACTION_H

/*
 *
 *
 */

#include "actions/details/omAction.h"

class OmSegmentJoinActionImpl;

class OmSegmentJoinAction : public OmAction {

public:
	OmSegmentJoinAction( const OmID, const OmSegIDsSet&);

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string &);

	boost::shared_ptr<OmSegmentJoinActionImpl> impl_;
};

#endif
