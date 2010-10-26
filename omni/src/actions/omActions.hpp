#ifndef OM_ACTIONS_HPP
#define OM_ACTIONS_HPP

#include "actions/details/omProjectSaveAction.h"
#include "actions/details/omSegmentGroupAction.h"
#include "actions/details/omSegmentJoinAction.h"
#include "actions/details/omSegmentSelectAction.h"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omSegmentationThresholdChangeAction.h"
#include "actions/details/omVoxelSetValueAction.h"

class OmActions {
public:

// project-related
	inline static void Save(){
		(new OmProjectSaveAction())->Run();
	}

// MST-related
	inline static void ChangeMSTthreshold(const OmID segmentationID,
										  const float threshold)
	{
		(new OmSegmentationThresholdChangeAction(segmentationID,
												 threshold))->Run();
	}

// segment-related
	inline static void ValidateSegment(const SegmentDataWrapper& sdw,
									   const bool valid){
		OmSegmentValidateAction::Validate(sdw, valid);
	}

	inline static void ValidateSegment(const SegmentationDataWrapper& sdw,
									   const bool valid){
		OmSegmentValidateAction::Validate(sdw, valid);
	}

	inline static void UncertainSegment(const SegmentDataWrapper& sdw,
										const bool uncertain){
		OmSegmentUncertainAction::SetUncertain(sdw, uncertain);
	}

	inline static void UncertainSegment(const SegmentationDataWrapper& sdw,
										const bool uncertain){
		OmSegmentUncertainAction::SetUncertain(sdw, uncertain);
	}

	inline static void JoinSegments(const OmID segmentationID,
									const OmSegIDsSet& ids){
		(new OmSegmentJoinAction(segmentationID, ids))->Run();
	}

	inline static void FindAndSplitSegments(const SegmentDataWrapper& sdw,
											OmViewGroupState* vgs){
		OmSegmentSplitAction::DoFindAndSplitSegment(sdw, vgs);
	}

	inline static void SelectSegments(const OmID segmentationId,
									  const OmSegIDsSet & mNewSelectedIdSet,
									  const OmSegIDsSet & mOldSelectedIdSet,
									  const OmID segmentJustSelected,
									  void* sender,
									  const std::string & comment,
									  const bool doScroll,
									  const bool addToRecentList)
	{
		(new OmSegmentSelectAction(segmentationId,
								   mNewSelectedIdSet,
								   mOldSelectedIdSet,
								   segmentJustSelected,
								   sender,
								   comment,
								   doScroll,
								   addToRecentList))->Run();
	}

// group-related
	inline static void CreateOrDeleteSegmentGroup(const OmID segmentationID,
												  const OmSegIDsSet& selectedSegmentIDs,
												  const OmGroupName name,
												  const bool create)
	{
		(new OmSegmentGroupAction(segmentationID,
								  selectedSegmentIDs,
								  name,
								  create))->Run();
	}
};

#endif
