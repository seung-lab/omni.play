#ifndef OM_SEGMENT_SELECT_ACTION_H
#define OM_SEGMENT_SELECT_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class OmSegmentSelectAction : public OmAction {

public:
	OmSegmentSelectAction(const OmId segmentationId, 
			      const OmSegIDsSet & mNewSelectedIdSet, 
			      const OmSegIDsSet & mOldSelectedIdSet, 
			      const OmId segmentJustSelected,
			      void* sender,
			      const string & comment,
			      const bool doScroll );
	
private:
	void Action();
	void UndoAction();
	string Description();
	void save(const string &);
	QString classNameForLogFile(){return "OmSegmentSelectAction";}
	
	OmId mSegmentationId;
	OmSegIDsSet mNewSelectedIdSet;
	OmSegIDsSet mOldSelectedIdSet;
	OmId mSegmentJustSelectedID;
	void * mSender;
	const string mComment;
	const bool mDoScroll;

	friend class OmActionLoggerFS;
	friend QDataStream &operator<<(QDataStream & out, const OmSegmentSelectAction & a );
	friend QDataStream &operator>>(QDataStream & in,  OmSegmentSelectAction & a );
};

#endif
