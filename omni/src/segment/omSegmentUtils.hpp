#ifndef OM_SEGMENT_UTILS_HPP
#define OM_SEGMENT_UTILS_HPP

#include "project/omSegmentationManager.h"
#include "segment/omSegmentSearched.hpp"
#include "segment/omSegment.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentCache.h"
#include "project/omProjectVolumes.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/segmentDataWrapper.hpp"

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

	boost::optional<DataBbox>
	static computeSelectedSegmentBoundingBox(const SegmentationDataWrapper& sdw)
	{
		DataBbox box;

		OmSegmentIterator iter(sdw.SegmentCache());
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
			return boost::optional<DataBbox>();
		}

		return boost::optional<DataBbox>(box);
	}

	boost::optional<DataCoord>
	static findCenterOfSelectedSegments(const SegmentationDataWrapper& sdw)
	{
		boost::optional<DataBbox> box = computeSelectedSegmentBoundingBox(sdw);

		if(!box){
			return boost::optional<DataCoord>();
		}

		const DataCoord ret = (box->getMin() + box->getMax()) / 2;
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
	template <typename T>
	static void CenterSegment(OmViewGroupState * vgs,
							  const T& sdw)
	{
		const boost::optional<DataCoord> voxelDC
			= findCenterOfSelectedSegments(sdw);

		if(!voxelDC){
			return;
		}

		const SpaceCoord voxelSC
			= sdw.GetSegmentation().Coords().DataToSpaceCoord(*voxelDC);

		vgs->SetViewSliceDepth(YZ_VIEW, voxelSC.x );
		vgs->SetViewSliceDepth(XZ_VIEW, voxelSC.y );
		vgs->SetViewSliceDepth(XY_VIEW, voxelSC.z );

		OmEvents::ViewCenterChanged();
		OmEvents::View3dRecenter();
	}

	// TODO: make more efficient
	static uint32_t NumberOfDescendants(const SegmentDataWrapper& sdw)
	{
		OmSegmentIterator iter(sdw.SegmentCache());
		iter.iterOverSegmentID(sdw.GetSegmentID());

		uint32_t counter = 0;

		OmSegment* seg = iter.getNextSegment();
		while(seg){
			++counter;
			seg = iter.getNextSegment();
		}

		return counter;
	}

	static bool UseParentColorBasedOnThreshold(OmSegment* seg,
											   OmViewGroupState* vgs)
	{
		return seg->getParent() &&
			seg->getThreshold() > vgs->getBreakThreshold() &&
			seg->getThreshold() < 2;
		// 2 is the manual merge threshold
	}

	static OmSegID
	GetNextSegIDinWorkingList(const SegmentDataWrapper& sdw)
	{
		if(!sdw.IsSegmentValid()){
			return 0;
		}

		const OmSegID rootID = sdw.FindRootID();
		const OmSegID nextID =
			sdw.SegmentLists()->Working().GetNextSegmentIDinList(rootID);

		if(sdw.SegmentCache()->IsSegmentValid(nextID)){
			return nextID;
		}
		return 0;
	}

	static OmSegment*
	GetNextSegInWorkingList(const SegmentDataWrapper& sdw)
	{
		const OmSegID nextID = GetNextSegIDinWorkingList(sdw);
		if(!nextID){
			return NULL;
		}

		return sdw.SegmentCache()->GetSegment(nextID);
	}

	/* iterate over selected segments; choose the segment
	 *  whoose following segment is the largest
	 */
	static OmSegID
	GetNextSegIDinWorkingList(const SegmentationDataWrapper& sdw_)
	{
		OmSegmentCache* segCache = sdw_.SegmentCache();

		OmSegment* biggestSeg = NULL;
		uint64_t biggestSegSize = 0;

		FOR_EACH(iter, segCache->GetSelectedSegmentIds()){
			SegmentDataWrapper sdw(sdw_, *iter);
			OmSegment* seg = GetNextSegInWorkingList(sdw);
			if(seg){
				const uint64_t size = seg->getSizeWithChildren();
				if(size > biggestSegSize){
					biggestSeg = seg;
					biggestSegSize = size;
				}
			}
		}

		if(biggestSeg && biggestSegSize){
			return biggestSeg->value();
		}
		return 0;
	}

	static boost::optional<float> ComputeCameraDistanceForSelectedSegments()
	{
       	DataBbox box;
		Vector3f res;

        FOR_EACH(iter, OmProject::Volumes().Segmentations().GetValidSegmentationIds()){
			SegmentationDataWrapper sdw(*iter);
			const boost::optional<DataBbox> b =
				computeSelectedSegmentBoundingBox(sdw);
			if(b){
				box.merge(*b);
			}
			res = sdw.GetDataResolution();
		}

		if(box.isEmpty()){
			return boost::optional<float>();
        }

		const float x = (box.getMax().x - box.getMin().x) * res.x;
		const float y = (box.getMax().y - box.getMin().y) * res.y;
		const float z = (box.getMax().z - box.getMin().z) * res.z;

		return boost::optional<float>(sqrt(x*x + y*y + z*z));
	}

	static void RebuildCenterOfSegmentData(const SegmentationDataWrapper& sdw)
	{
		printf("rebuilding segment bounding box data...\n");

		OmSegmentCache* segCache = sdw.SegmentCache();
		for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
			OmSegment* seg = segCache->GetSegment(i);
			if(!seg){
				continue;
			}
			seg->clearBounds();
		}

		sdw.GetSegmentation().UpdateVoxelBoundingData();
		sdw.GetSegmentation().Flush();
	}
};
#endif
