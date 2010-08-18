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

	OmId mSegmentationId;
	OmSegIDsSet mSelectedSegmentIds;

	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream & out, const OmSegmentJoinAction & a );
	friend class QDataStream &operator>>(QDataStream & in,  OmSegmentJoinAction & a );
};

#endif
