#ifndef OM_SEGMENTATION_THRESHOLD_CHANGE_ACTION_H
#define OM_SEGMENTATION_THRESHOLD_CHANGE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class OmSegmentationThresholdChangeAction : public OmAction {

public:
	OmSegmentationThresholdChangeAction( const OmId segmentationId, const float threshold);

private:
	void Action();
	void UndoAction();
	string Description();
	void save(const string &);
	QString classNameForLogFile(){return "OmSegmentationThresholdChangeAction";}

	OmId mSegmentationId;
	float mThreshold;
	float mOldThreshold;

	friend class OmActionLoggerFS;
	friend class QDataStream &operator<<(QDataStream & out, const OmSegmentationThresholdChangeAction & a );
	friend class QDataStream &operator>>(QDataStream & in,  OmSegmentationThresholdChangeAction & a );
};

#endif
