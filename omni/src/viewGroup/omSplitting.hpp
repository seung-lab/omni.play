#ifndef OM_SPLITTING_HPP
#define OM_SPLITTING_HPP

#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/cache/omCacheManager.h"
#include "system/omEvents.h"

class OmSplitting {
private:
    bool splitting_;
    bool showSplit_;
    ToolBarManager* toolBarManager_;
    boost::optional<DataCoord> coordBeingSplit_;
    SegmentDataWrapper segmentBeingSplit_;

public:
    OmSplitting()
        : splitting_(false)
        , showSplit_(false)
        , toolBarManager_(NULL)
    {}

    inline bool ShowSplit() const {
        return showSplit_;
    }

    const SegmentDataWrapper& Segment() const {
        return segmentBeingSplit_;
    }

    const boost::optional<DataCoord>& Coord() const {
        return coordBeingSplit_;
    }

    void SetToolBarManager(ToolBarManager* tbm){
        toolBarManager_ = tbm;
    }

    void SetShowSplitMode(const bool mode)
    {
        OmCacheManager::TouchFresheness();
        OmEvents::Redraw3d();
        OmEvents::Redraw2d();
        showSplit_ = mode;
    }

    void SetSplitMode(const bool onoroff, const bool postEvent = true)
    {
        splitting_ = onoroff;
        if(false == onoroff){
            if(postEvent) {
                toolBarManager_->SetSplittingOff();
            }
            SetShowSplitMode(false);
            OmStateManager::SetOldToolModeAndSendEvent();
            coordBeingSplit_.reset();
        }

        OmCacheManager::TouchFresheness();
        OmEvents::Redraw3d();
        OmEvents::Redraw2d();
    }

    void SetSplitMode(const SegmentDataWrapper& sdw, const DataCoord& coord)
    {
        segmentBeingSplit_ = sdw;
        coordBeingSplit_ = boost::optional<DataCoord>(coord);
        SetSplitMode(true);
    }
};

#endif
