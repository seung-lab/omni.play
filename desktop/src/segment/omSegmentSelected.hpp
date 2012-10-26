#pragma once

#include "actions/omActions.h"
#include "common/common.h"
#include "events/omEvents.h"
#include "segment/omSegmentSelector.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "zi/omUtility.h"

class OmSegmentSelected : private om::singletonBase<OmSegmentSelected> {
public:
    static void Delete(){
        instance().sdw_ = SegmentDataWrapper();
    }

    static void Set(const SegmentDataWrapper& sdw){
        instance().sdw_ = sdw;
    }

    static SegmentDataWrapper Get(){
        return instance().sdw_;
    }

    static SegmentDataWrapper GetSegmentForPainting(){
        return instance().sdwForPainting_;
    }

    static void SetSegmentForPainting(const SegmentDataWrapper& sdw)
    {
        instance().sdwForPainting_ = sdw;
        OmEvents::SegmentSelected();
    }

    static void RandomizeColor()
    {
        if(!instance().sdw_.IsSegmentValid()){
            return;
        }
        instance().sdw_.RandomizeColor();
        OmCacheManager::TouchFreshness();
        OmEvents::Redraw2d();
    }

    static void AugmentSelection(const SegmentDataWrapper& sdw)
    {
        OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), NULL, "OmSegmentSelected");
        sel.augmentSelectedSet(sdw.getID(), true);
        sel.sendEvent();
    }

    static void ToggleValid()
    {
        if(!instance().sdw_.IsSegmentValid()){
            return;
        }

        OmSegment* seg = instance().sdw_.GetSegment();

        if(seg->IsValidListType()){
            OmActions::ValidateSegment(instance().sdw_, om::SET_NOT_VALID);
        } else {
            OmActions::ValidateSegment(instance().sdw_, om::SET_VALID);
        }

        OmEvents::SegmentModified();
    }

private:
    OmSegmentSelected(){}
    ~OmSegmentSelected(){}

    SegmentDataWrapper sdw_;
    SegmentDataWrapper sdwForPainting_;

    friend class zi::singleton<OmSegmentSelected>;
};

