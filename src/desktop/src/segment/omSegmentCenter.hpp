#pragma once

#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegmentIterator.h"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"

class OmSegmentCenter {
private:

    boost::optional<DataBbox>
    static computeSelectedSegmentBoundingBox(const SegmentationDataWrapper& sdw)
    {
        DataBbox box;

        OmSegmentIterator iter(sdw.Segments());
        iter.iterOverSelectedIDs();

        const int max = 5000;

        OmSegment* seg = iter.getNextSegment();
        for(int i = 0; i < max && NULL != seg; ++i)
        {
            const DataBbox segBox = seg->BoundingBox();
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

        OmSegment* seg = sdw.GetSegment();
        const DataBbox& box = seg->BoundingBox();
        if(box.isEmpty()){
            return boost::optional<DataCoord>();
        }

        const DataCoord ret = (box.getMin() + box.getMax()) / 2;
        return boost::optional<DataCoord>(ret);
    }

public:
    static void CenterSegment(OmViewGroupState* vgs)
    {
        const SegmentationDataWrapper sdw = vgs->Segmentation();

        const boost::optional<DataCoord> voxelDC
            = findCenterOfSelectedSegments(sdw);

        if(!voxelDC){
            return;
        }

        // printf("v: %i %i %i\n", voxelDC->x, voxelDC->y, voxelDC->z);

        OmSegmentation& segmentation = sdw.GetSegmentation();
        const Vector3f& res = segmentation.Coords().GetDataResolution();

        const Vector3f newLoc = *voxelDC * res;

        vgs->View2dState()->SetScaledSliceDepth(newLoc);

        OmEvents::ViewCenterChanged();
        OmEvents::View3dRecenter();
    }

    static boost::optional<float> ComputeCameraDistanceForSelectedSegments()
    {
       	DataBbox box;
        Vector3f res;

        FOR_EACH(iter, SegmentationDataWrapper::ValidIDs())
        {
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

        OmSegments* segments = sdw.Segments();

        for(OmSegID i = 1; i <= segments->getMaxValue(); ++i)
        {
            OmSegment* seg = segments->GetSegment(i);
            if(!seg){
                continue;
            }

            seg->ClearBoundingBox();
        }

        sdw.GetSegmentation().UpdateVoxelBoundingData();
        sdw.GetSegmentation().Flush();
    }
};

