#ifndef OM_SEGMENT_UTILS_HPP
#define OM_SEGMENT_UTILS_HPP

#include "segment/omSegmentSearched.hpp"
#include "segment/omSegment.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentCache.h"
#include "viewGroup/omViewGroupState.h"

class OmSegmentUtils {
public:
	boost::shared_ptr<std::set<OmSegment*> >
	static GetAllChildrenSegments(OmSegmentCache* segCache,
								  const OmSegIDsSet& set)
	{
		OmSegmentIterator iter(segCache);
		iter.iterOverSegmentIDs(set);

		OmSegment * seg = iter.getNextSegment();
		std::set<OmSegment*>* children = new std::set<OmSegment*>();

		while(NULL != seg) {
			children->insert(seg);
			seg = iter.getNextSegment();
		}

		return boost::shared_ptr<std::set<OmSegment*> >(children);
	}

private:
	boost::optional<DataCoord>
	static findCenterOfSelectedSegments(const SegmentationDataWrapper& sdw)
	{
		DataBbox box;

		OmSegmentIterator iter(sdw.GetSegmentCache());
		iter.iterOverSelectedIDs();

		const int max = 5000;

		OmSegment* seg = iter.getNextSegment();
		for(int i = 0; i < max && NULL != seg; ++i){

			const DataBbox& segBox = seg->getBounds();
			if(segBox.isEmpty()){
				continue;
			}

			box.merge(segBox);

			seg = iter.getNextSegment();
		}

		if(box.isEmpty()){
			return boost::optional<DataCoord>();
		}

		const DataCoord ret = (box.getMin() + box.getMax()) / 2;
		return boost::optional<DataCoord>(ret);
	}

	boost::optional<DataCoord>
	static findCenterOfSelectedSegments(const SegmentDataWrapper& sdw)
	{
		if(!sdw.IsSegmentValid()){
			return boost::optional<DataCoord>();
		}

		OmSegment* seg = sdw.getSegment();
		const DataBbox& box = seg->getBounds();
		if(box.isEmpty()){
			return boost::optional<DataCoord>();
		}

		const DataCoord ret = (box.getMin() + box.getMax()) / 2;
		return boost::optional<DataCoord>(ret);
	}

public:
	static void CenterSegment(OmViewGroupState * vgs,
							  const SegmentationDataWrapper& sdw)
	{

		const boost::optional<DataCoord> voxelDC
			= findCenterOfSelectedSegments(sdw);

		if(!voxelDC){
			return;
		}

		const SpaceCoord voxelSC
			= sdw.GetSegmentation().DataToSpaceCoord(*voxelDC);

		vgs->SetViewSliceDepth(YZ_VIEW, voxelSC.x );
		vgs->SetViewSliceDepth(XZ_VIEW, voxelSC.y );
		vgs->SetViewSliceDepth(XY_VIEW, voxelSC.z );

		OmEvents::ViewCenterChanged();
		OmEvents::View3dRecenter();
	}

	static void CenterSegment(OmViewGroupState * vgs,
							  const SegmentDataWrapper& sdw)
	{

		const boost::optional<DataCoord> voxelDC
			= findCenterOfSelectedSegments(sdw);

		if(!voxelDC){
			return;
		}

		const SpaceCoord voxelSC
			= sdw.GetSegmentation().DataToSpaceCoord(*voxelDC);

		vgs->SetViewSliceDepth(YZ_VIEW, voxelSC.x );
		vgs->SetViewSliceDepth(XZ_VIEW, voxelSC.y );
		vgs->SetViewSliceDepth(XY_VIEW, voxelSC.z );

		OmEvents::ViewCenterChanged();
		OmEvents::View3dRecenter();
	}

	// TODO: make more efficient
	static uint32_t NumberOfDescendants(const SegmentDataWrapper& sdw)
	{
		OmSegmentIterator iter(sdw.GetSegmentCache());
		iter.iterOverSegmentID(sdw.GetSegmentID());

		uint32_t counter = 0;

		OmSegment* seg = iter.getNextSegment();
		while(seg){
			++counter;
			seg = iter.getNextSegment();
		}

		return counter;
	}

	static void RebuildCenterOfSegmentData(const SegmentationDataWrapper& sdw)
	{
		printf("rebuilding segment bounding box data...\n");

		OmSegmentCache* segCache = sdw.GetSegmentCache();
		for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
			OmSegment* seg = segCache->GetSegment(i);
			if(!seg){
				continue;
			}
			seg->clearBounds();
		}

		sdw.GetSegmentation().UpdateVoxelBoundingData();
	}

};

#endif
