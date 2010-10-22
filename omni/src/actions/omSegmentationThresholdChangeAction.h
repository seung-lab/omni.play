#ifndef OM_SEGMENTATION_THRESHOLD_CHANGE_ACTION_H
#define OM_SEGMENTATION_THRESHOLD_CHANGE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class OmSegmentationThresholdChangeAction : public OmAction {

public:
	OmSegmentationThresholdChangeAction( const OmID segmentationId, const float threshold);

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string &);
	QString classNameForLogFile(){return "OmSegmentationThresholdChangeAction";}

	OmID mSegmentationId;
	float mThreshold;
	float mOldThreshold;

	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream & out, const OmSegmentationThresholdChangeAction & a );
	friend class QDataStream &operator>>(QDataStream & in,  OmSegmentationThresholdChangeAction & a );
};

#endif
