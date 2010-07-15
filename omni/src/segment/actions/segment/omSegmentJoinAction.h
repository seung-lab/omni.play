#ifndef OM_SEGMENT_JOIN_ACTION_H
#define OM_SEGMENT_JOIN_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class OmSegmentJoinAction : public OmAction {

public:
	OmSegmentJoinAction( const OmId segmentationId, const OmSegIDsSet & selectedSegmentIdsSet);

private:
	void Action();
	void UndoAction();
	string Description();
	void save(const string &);
	QString classNameForLogFile(){return "OmSegmentJoinAction";}

	const OmId mSegmentationId;
	OmSegIDsSet mSelectedSegmentIds;

	friend class OmActionLoggerFS;
};

#endif
