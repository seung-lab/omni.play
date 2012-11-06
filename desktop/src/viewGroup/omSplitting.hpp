#pragma once

#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/cache/omCacheManager.h"
#include "events/omEvents.h"
#include "gui/tools.hpp"

class OmSplitting {
private:
    bool showSplit_;
    ToolBarManager* toolBarManager_;

    boost::optional<om::coords::Global> coordBeingSplit_;
    SegmentDataWrapper segmentBeingSplit_;

public:
    OmSplitting()
        : showSplit_(false)
        , toolBarManager_(NULL)
    {}

    inline bool ShowSplit() const {
        return showSplit_;
    }

    const SegmentDataWrapper& Segment() const {
        return segmentBeingSplit_;
    }

    const boost::optional<om::coords::Global>& Coord() const {
        return coordBeingSplit_;
    }

    void SetToolBarManager(ToolBarManager* tbm){
        toolBarManager_ = tbm;
    }

    void EnterSplitMode()
    {
        showSplit_ = true;

        OmStateManager::SetToolModeAndSendEvent(om::tool::SPLIT);
        OmEvents::Redraw3d();
        OmEvents::Redraw2d();
    }

    void ExitSplitMode()
    {
        showSplit_ = false;
        coordBeingSplit_.reset();

        OmStateManager::SetOldToolModeAndSendEvent();
        OmEvents::Redraw3d();
        OmEvents::Redraw2d();
    }

    void ExitSplitModeFixButton()
    {
        toolBarManager_->SetSplittingOff();
        ExitSplitMode();
    }

    void SetFirstSplitPoint(const SegmentDataWrapper& sdw, const om::coords::Global& coord)
    {
        segmentBeingSplit_ = sdw;
        coordBeingSplit_ = boost::optional<om::coords::Global>(coord);
    }
};

