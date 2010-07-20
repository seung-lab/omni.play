#ifndef OM_SEGMENT_VALIDATE_ACTION_H
#define OM_SEGMENT_VALIDATE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "common/omCommon.h"

class OmSegmentValidateAction : public OmAction {

public:
	OmSegmentValidateAction( const OmId segmentationId, const OmSegIDsSet & selectedSegmentIdsSet, const bool create);

private:
	void Action();
	void UndoAction();
	string Description();
	void save(const string &);
	QString classNameForLogFile(){return "OmSegmentValidateAction";}

	OmId mSegmentationId;
	bool mCreate;
	OmSegIDsSet mSelectedSegmentIds;

        friend QDataStream &operator<<(QDataStream & out, const OmSegmentValidateAction & action );
        friend QDataStream &operator>>(QDataStream & in, OmSegmentValidateAction & action );

	friend class OmActionLoggerFS;
};

#endif
