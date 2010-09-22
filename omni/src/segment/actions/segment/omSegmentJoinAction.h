#ifndef OM_SEGMENT_JOIN_ACTION_H
#define OM_SEGMENT_JOIN_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class OmSegmentJoinAction : public OmAction {

public:
	OmSegmentJoinAction( const OmId, const OmSegIDsSet&);

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string &);
	QString classNameForLogFile(){return "OmSegmentJoinAction";}

	OmId mSegmentationId;
	OmSegIDsSet mSelectedSegmentIds;

	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream&, const OmSegmentJoinAction&);
	friend class QDataStream &operator>>(QDataStream&, OmSegmentJoinAction&);
};

#endif
