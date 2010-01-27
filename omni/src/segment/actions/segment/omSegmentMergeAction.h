#ifndef OM_SEGMENT_MERGE_ACTION_H
#define OM_SEGMENT_MERGE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "system/omSystemTypes.h"

#include "volume/omVolumeTypes.h"
#include "segment/omSegmentTypes.h"

class OmSegmentMergeAction:public OmAction {

 public:
	OmSegmentMergeAction(OmId mergeSegmentationId);

 private:
	void Action();
	void UndoAction();
	string Description();

	OmId mMergeSegmentationId;
	OmId mDestinationSegmentId;

	 map < OmId, SegmentDataSet > mPrevSegmentIdToValueMap;
	SegmentDataSet mMergedDataSet;

};

#endif
