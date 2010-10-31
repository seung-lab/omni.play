#ifndef OM_SEGMENT_UNCERTAIN_ACTION_H
#define OM_SEGMENT_UNCERTAIN_ACTION_H

/*
 *
 *
 */

#include "actions/details/omAction.h"
#include "common/omCommon.h"

class OmSegment;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentUncertainActionImpl;

class OmSegmentUncertainAction : public OmAction {

public:
	static void SetUncertain(const SegmentDataWrapper& sdw,
							 const bool uncertain);

	static void SetUncertain(const SegmentationDataWrapper& sdw,
							 const bool uncertain);

private:
	OmSegmentUncertainAction(const OmID segmentationId,
							 boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
							 const bool valid);
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);

	boost::shared_ptr<OmSegmentUncertainActionImpl> impl_;

};

#endif
