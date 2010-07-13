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

	const OmId mSegmentationId;
	const bool mCreate;
	OmSegIDsSet mSelectedSegmentIds;
};

#endif
