#include "actions/io/omActionOperators.h"
#include "datalayer/archive/old/omDataArchiveBoost.h"
#include "segment/omSegment.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "datalayer/archive/old/omDataArchiveWrappers.h"

#include "actions/details/omSegmentGroupActionImpl.hpp"
#include "actions/details/omSegmentJoinActionImpl.hpp"
#include "actions/details/omSegmentSelectActionImpl.hpp"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "actions/details/omSegmentCutActionImpl.hpp"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "actions/details/omSegmentUncertainActionImpl.hpp"
#include "actions/details/omSegmentationThresholdChangeActionImpl.hpp"
#include "actions/details/omSegmentationSizeThresholdChangeActionImpl.hpp"
#include "actions/details/omVoxelSetValueActionImpl.hpp"
#include "actions/details/omProjectCloseActionImpl.hpp"

QDataStream& operator<<(QDataStream& out, const OmSegmentValidateActionImpl& a)
{
    int version = 1;
    out << version;

    OmSegIDsSet ids;
    std::set<OmSegment*>* segs = a.selectedSegments_.get();
    FOR_EACH(iter, *segs){
        OmSegment* seg = *iter;
        ids.insert(seg->value());
    }
    out << ids;
    out << a.valid_;
    out << a.sdw_.GetSegmentationID();

    return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentValidateActionImpl& a)
{
    int version;
    in >> version;

    OmSegIDsSet ids;
    in >> ids;
    in >> a.valid_;

    in >> a.sdw_;

    OmSegments* cache = a.sdw_.Segments();

    om::shared_ptr<std::set<OmSegment*> > segs =
        om::make_shared<std::set<OmSegment*> >();

    FOR_EACH(iter, ids){
        OmSegment* seg = cache->GetSegment(*iter);
        segs->insert(seg);
    }

    a.selectedSegments_ = segs;

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentUncertainActionImpl& a)
{
    int version = 1;
    out << version;

    OmSegIDsSet ids;
    std::set<OmSegment*>* segs = a.selectedSegments_.get();
    FOR_EACH(iter, *segs){
        OmSegment* seg = *iter;
        ids.insert(seg->value());
    }
    out << ids;
    out << a.uncertain_;
    out << a.sdw_;

    return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentUncertainActionImpl& a)
{
    int version;
    in >> version;

    OmSegIDsSet ids;
    in >> ids;
    in >> a.uncertain_;

    in >> a.sdw_;

    OmSegments* cache = a.sdw_.Segments();

    om::shared_ptr<std::set<OmSegment*> > segs =
        om::make_shared<std::set<OmSegment*> >();

    FOR_EACH(iter, ids){
        OmSegment* seg = cache->GetSegment(*iter);
        segs->insert(seg);
    }

    a.selectedSegments_ = segs;

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSplitActionImpl& a)
{
    int version = 3;
    out << version;
    out << a.mEdge;
    out << a.mSegmentationID;
    out << a.desc;

    return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentSplitActionImpl& a)
{
    int version;
    in >> version;

    in >> a.mEdge;
    in >> a.mSegmentationID;
    in >> a.desc;

    if(2 == version)
    {
        OmSegID deadSeg;
        in >> deadSeg;

        DataCoord dc;
        in >> dc;
        in >> dc;
    }

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentGroupActionImpl& a)
{
    int version = 1;
    out << version;
    out << a.mSegmentationId;
    out << a.mName;
    out << a.mCreate;
    out << a.mSelectedSegmentIds;

    return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentGroupActionImpl& a)
{
    int version;
    in >> version;
    in >> a.mSegmentationId;
    in >> a.mName;
    in >> a.mCreate;
    in >> a.mSelectedSegmentIds;

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentJoinActionImpl& a)
{
    int version = 1;
    out << version;
    out << a.sdw_;
    out << a.segIDs_;

    return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentJoinActionImpl& a)
{
    int version;
    in >> version;
    in >> a.sdw_;
    in >> a.segIDs_;

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSelectActionImpl& a)
{
    int version = 2;
    out << version;
    out << a.params_->sdw.GetSegmentationID();
    out << a.params_->newSelectedIDs;
    out << a.params_->oldSelectedIDs;
    out << a.params_->sdw.GetSegmentID();
    out << a.params_->augmentListOnly;

    return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentSelectActionImpl& a)
{
    int version;
    in >> version;

    om::shared_ptr<OmSelectSegmentsParams> params =
        om::make_shared<OmSelectSegmentsParams>();

    OmID segmentationID;
    in >> segmentationID;

    in >> params->newSelectedIDs;
    in >> params->oldSelectedIDs;

    OmSegID segmentID;
    in >> segmentID;

    params->sdw = SegmentDataWrapper(segmentationID, segmentID);

    if(version > 1){
        in >> params->augmentListOnly;
    }

    a.params_ = params;

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmProjectSaveActionImpl&)
{
    int version = 1;
    out << version;

    return out;
}

QDataStream& operator>>(QDataStream& in,   OmProjectSaveActionImpl& )
{
    int version;
    in >> version;

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmProjectCloseActionImpl&)
{
    int version = 1;
    out << version;

    return out;
}

QDataStream& operator>>(QDataStream& in,   OmProjectCloseActionImpl& )
{
    int version;
    in >> version;

    return in;
}


QDataStream& operator<<(QDataStream& out, const OmSegmentationThresholdChangeActionImpl& a)
{
    int version = 2;
    out << version;
    out << a.threshold_;
    out << a.oldThreshold_;
    out << a.sdw_;

    return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentationThresholdChangeActionImpl& a)
{
    int version;
    in >> version;
    in >> a.threshold_;
    in >> a.oldThreshold_;

    OmID id = 1;

    if(version > 1){
        in >> id;
    } else {
        printf("WARNGING: guessing segmentation ID...\n");
    }

    a.sdw_ = SegmentationDataWrapper(id);

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentationSizeThresholdChangeActionImpl& a)
{
    int version = 2;
    out << version;
    out << a.threshold_;
    out << a.oldThreshold_;
    out << a.sdw_;
    
    return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentationSizeThresholdChangeActionImpl& a)
{
    int version;
    in >> version;
    in >> a.threshold_;
    in >> a.oldThreshold_;
    
    OmID id = 1;
    
    if(version > 1){
        in >> id;
    } else {
        printf("WARNGING: guessing segmentation ID...\n");
    }
    
    a.sdw_ = SegmentationDataWrapper(id);
    
    return in;
}

QDataStream& operator<<(QDataStream& out, const OmVoxelSetValueActionImpl& a)
{
    int version = 1;
    out << version;
    out << a.mSegmentationId;
    //out << a.mOldVoxelValues;	//FIXME
    out << a.mNewValue;

    return out;
}

QDataStream& operator>>(QDataStream& in,  OmVoxelSetValueActionImpl& a)
{
    int version;
    in >> version;
    in >> a.mSegmentationId;
    //in >> a.mOldVoxelValues;	//FIXME
    in >> a.mNewValue;

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentCutActionImpl& a)
{
    int version = 1;
    out << version;

	out << a.sdw_;
    out << a.edges_;

    return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentCutActionImpl& a)
{
    int version;
    in >> version;

	in >> a.sdw_;
    in >> a.edges_;

    return in;
}
