#ifndef OM_ACTIONS_HPP
#define OM_ACTIONS_HPP

#include "common/om.hpp"
#include "common/omCommon.h"

class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmViewGroupState;

class OmActions {
public:

// project-related
	static void Save();

// MST-related
	static void ChangeMSTthreshold(const OmID segmentationID,
								   const float threshold);

// painting-related
	static void SetVoxel(const OmID segmentationId,
						 const DataCoord& rVoxel,
						 const OmSegID value);

	static void SetVoxels(const OmID segmentationId,
						  const std::set<DataCoord>& rVoxels,
						  const OmSegID value);

// segment-related
	static void ValidateSegment(const SegmentDataWrapper& sdw,
								const om::SetValid valid);

	static void ValidateSegment(const SegmentationDataWrapper& sdw,
								const om::SetValid valid);

	static void UncertainSegment(const SegmentDataWrapper& sdw,
								 const bool uncertain);

	static void UncertainSegment(const SegmentationDataWrapper& sdw,
								 const bool uncertain);

	static void JoinSegments(const OmID segmentationID,
							 const OmSegIDsSet& ids);

	static void FindAndSplitSegments(const SegmentDataWrapper& sdw,
									 OmViewGroupState* vgs);

	static void SelectSegments(const OmID segmentationId,
							   const OmSegIDsSet & mNewSelectedIdSet,
							   const OmSegIDsSet & mOldSelectedIdSet,
							   const OmID segmentJustSelected,
							   void* sender,
							   const std::string & comment,
							   const bool doScroll,
							   const bool addToRecentList);

// group-related
	static void CreateOrDeleteSegmentGroup(const OmID segmentationID,
										   const OmSegIDsSet& selectedSegmentIDs,
										   const OmGroupName name,
										   const bool create);
};

#endif
