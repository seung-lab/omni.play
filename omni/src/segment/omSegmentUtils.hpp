#ifndef OM_SEGMENT_UTILS_HPP
#define OM_SEGMENT_UTILS_HPP

#include "utility/dataWrappers.h"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegmentSearched.hpp"
#include "segment/omSegmentSelected.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"

class OmSegmentUtils {
public:
    boost::shared_ptr<std::set<OmSegment*> >
    static GetAllChildrenSegments(OmSegments* segments,
                                  const OmSegIDsSet& set)
    {
        OmSegmentIterator iter(segments);
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

        OmSegmentIterator iter(sdw.Segments());
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

        OmSegment* seg = sdw.GetSegment();
        const DataBbox& box = seg->getBounds();
        if(box.isEmpty()){
            return boost::optional<DataCoord>();
        }

        const DataCoord ret = (box.getMin() + box.getMax()) / 2;
        return boost::optional<DataCoord>(ret);
    }

public:
    template <typename T>
    static void CenterSegment(OmViewGroupState * vgs, const T& sdw)
    {
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

    static OmSegment* GetSegmentBasedOnThreshold(OmSegment* seg, const float breakThreshold)
    {
        // WARNING: operation is O(depth of MST)

        OmSegment* segWalker = seg;

        OmSegment* parent = segWalker->getParent();
        float segThreshold = segWalker->getThreshold();

        while(parent &&
              segThreshold > breakThreshold &&
              segThreshold < 2)
        {
            segWalker = parent;
            segThreshold = segWalker->getThreshold();
            parent = segWalker->getParent();
        }

        return segWalker;
    }

    inline static bool UseParentColorBasedOnThreshold(OmSegment* seg,
                                                      const float breakThreshold)
    {
        return seg->getParent() &&
            seg->getThreshold() > breakThreshold &&
            seg->getThreshold() < 2;
        // 2 is the manual merge threshold
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

        OmSegments* segments = sdw.Segments();
        for(OmSegID i = 1; i <= segments->getMaxValue(); ++i){
            OmSegment* seg = segments->GetSegment(i);
            if(!seg){
                continue;
            }
            seg->clearBounds();
        }

        sdw.GetSegmentation().UpdateVoxelBoundingData();
        sdw.GetSegmentation().Flush();
    }

    static void PrintChildren(const SegmentDataWrapper& sdw)
    {
        if(!sdw.IsSegmentValid()){
            return;
        }

        OmSegments* segments = sdw.Segments();
        OmSegmentIterator iter(segments);
        iter.iterOverSegmentID(sdw.FindRootID());

        OmSegment * seg = iter.getNextSegment();
        while(NULL != seg) {
            OmSegment* parent = seg->getParent();
            OmSegID parentID = 0;
            if(parent){
                parentID = parent->value();
            }
            const QString str = QString("%1 : %2, %3, %4")
                .arg(seg->value())
                .arg(parentID)
                .arg(seg->getThreshold())
                .arg(seg->size());
            printf("%s\n", qPrintable(str));
            seg = iter.getNextSegment();
        }
    }

    static void ReValidateEveryObject(const SegmentationDataWrapper& sdw)
    {
        OmSegments* segments = sdw.Segments();

        OmValidGroupNum* validGroupNum = sdw.ValidGroupNum();

        for(uint32_t i = 1; i <= segments->getMaxValue(); ++i)
        {
            OmSegment* seg = segments->GetSegment(i);

            // only process valid, root (i.e. parent == 0) segments
            if(!seg || !seg->IsValidListType() || seg->getParent()){
                continue;
            }

            OmSegIDsSet set;
            set.insert(i);

            boost::shared_ptr<std::set<OmSegment*> > children =
                OmSegmentUtils::GetAllChildrenSegments(segments, set);

            validGroupNum->Set(children, true);
        }
    }

    static OmSegID GetNextSegIDinWorkingList(const SegmentDataWrapper& sdw){
        return sdw.GetSegmentation().SegmentLists()->GetNextSegIDinWorkingList(sdw);
    }

    static OmSegID GetNextSegIDinWorkingList(const SegmentationDataWrapper& sdw){
        return sdw.SegmentLists()->GetNextSegIDinWorkingList(sdw);
    }

    static std::string ListTypeAsStr(const om::SegListType type)
    {
        switch(type){
        case om::WORKING:
            return "working";
        case om::VALID:
            return "valid";
        case om::UNCERTAIN:
            return "uncertain";
        default:
            return "unknown type";
        }
    }
};
#endif
