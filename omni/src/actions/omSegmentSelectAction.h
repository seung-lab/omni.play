#ifndef OM_SEGMENT_SELECT_ACTION_H
#define OM_SEGMENT_SELECT_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class OmSegmentSelectAction : public OmAction {

public:
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
	QString classNameForLogFile(){return "OmSegmentSelectAction";}

	OmID mSegmentationId;
	OmSegIDsSet mNewSelectedIdSet;
	OmSegIDsSet mOldSelectedIdSet;
	OmID mSegmentJustSelectedID;
	void * mSender;
	const std::string mComment;
	const bool mDoScroll;
	const bool mAddToRecentList;

	template <typename T> friend class OmActionLoggerFSThread;
	friend QDataStream &operator<<(QDataStream & out, const OmSegmentSelectAction & a );
	friend QDataStream &operator>>(QDataStream & in,  OmSegmentSelectAction & a );
};

#endif
