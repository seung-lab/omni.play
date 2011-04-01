#ifndef OM_SPLITTING_HPP
#define OM_SPLITTING_HPP

#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/cache/omCacheManager.h"
#include "events/omEvents.h"

class OmSplitting {
private:
    bool showSplit_;
    ToolBarManager* toolBarManager_;

    boost::optional<DataCoord> coordBeingSplit_;
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

    const boost::optional<DataCoord>& Coord() const {
        return coordBeingSplit_;
    }

    void ShowBusy(const bool showBusy){
        toolBarManager_->ShowSplitterBusy(showBusy);
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

    void SetFirstSplitPoint(const SegmentDataWrapper& sdw, const DataCoord& coord)
    {
        segmentBeingSplit_ = sdw;
        coordBeingSplit_ = boost::optional<DataCoord>(coord);
    }
};

#endif
