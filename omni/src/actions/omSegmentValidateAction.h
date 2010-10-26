#ifndef OM_SEGMENT_VALIDATE_ACTION_H
#define OM_SEGMENT_VALIDATE_ACTION_H

#include "system/omAction.h"
#include "common/omCommon.h"

class OmSegment;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentValidateActionImpl;

class OmSegmentValidateAction : public OmAction {

public:
	static void Validate(const SegmentDataWrapper& sdw,
						 const bool valid);

	static void Validate(const SegmentationDataWrapper& sdw,
						 const bool valid);

private:
	OmSegmentValidateAction(const OmID segmentationId,
							boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
							const bool valid);
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);

	boost::shared_ptr<OmSegmentValidateActionImpl> impl_;

};

#endif
