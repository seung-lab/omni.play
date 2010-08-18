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
	void save(const string &);
	QString classNameForLogFile(){return "OmSegmentGroupAction";}

	OmId mSegmentationId;
	OmGroupName mName;
	bool mCreate;
	OmSegIDsSet mSelectedSegmentIds;

	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream & out, const OmSegmentGroupAction & a );
	friend class QDataStream &operator>>(QDataStream & in,  OmSegmentGroupAction & a );
};

#endif
