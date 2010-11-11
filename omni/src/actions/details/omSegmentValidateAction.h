#ifndef OM_SEGMENT_VALIDATE_ACTION_H
#define OM_SEGMENT_VALIDATE_ACTION_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "actions/details/omAction.h"

class OmSegment;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentValidateActionImpl;

class OmSegmentValidateAction : public OmAction {

public:
	static void Validate(const SegmentDataWrapper& sdw,
						 const om::SetValid valid);

	static void Validate(const SegmentationDataWrapper& sdw,
						 const om::SetValid valid);

private:
	OmSegmentValidateAction(const SegmentationDataWrapper& sdw,
							boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
							const bool valid);
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);

	boost::shared_ptr<OmSegmentValidateActionImpl> impl_;

};

#endif
