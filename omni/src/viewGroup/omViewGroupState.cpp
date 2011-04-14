#include "common/omDebug.h"
#include "events/omEvents.h"
#include "gui/toolbars/toolbarManager.h"
#include "gui/viewGroup/viewGroup.h"
#include "segment/colorizer/omSegmentColorizer.h"
#include "segment/omSegment.h"
#include "system/cache/omCacheManager.h"
#include "system/omStateManager.h"
#include "tiles/omTileCoord.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omColorizers.hpp"
#include "viewGroup/omCutting.hpp"
#include "viewGroup/omSplitting.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "viewGroup/omZoomLevel.hpp"

OmViewGroupState::OmViewGroupState(MainWindow* mainWindow)
    : OmManageableObject()
    , viewGroup_(new ViewGroup(mainWindow, this))
    , view2dState_(new OmViewGroupView2dState())
    , colorizers_(new OmColorizers(this))
    , zoomLevel_(new OmZoomLevel())
    , splitting_(new OmSplitting())
    , cutting_(new OmCutting())
    , toolBarManager_(NULL)
{
    mBreakThreshold = 0;
    mDustThreshold = 90;
    mShatter = false;
    mShowValid = false;
    mShowValidInColor = false;
    mShowFilterInColor = false;
}

OmViewGroupState::~OmViewGroupState()
{}

OmPooledTile<OmColorARGB>* OmViewGroupState::ColorTile(uint32_t const* imageData,
                                                       const Vector2i& dims,
                                                       const OmTileCoord& key)
{
    return colorizers_->ColorTile(imageData, dims, key);
}

OmSegmentColorCacheType
OmViewGroupState::determineColorizationType(const ObjectType objType)
{
    switch(objType){
    case CHANNEL:
        if(mShowValid)
        {
            if(mShowValidInColor){
                return SCC_FILTER_VALID;
            }
            return SCC_FILTER_VALID_BLACK;
        }

        if(shouldVolumeBeShownBroken()){
            return SCC_FILTER_BREAK;
        }

        if(mShowFilterInColor){
            return SCC_FILTER_COLOR;
        }

        return SCC_FILTER_BLACK;

    case SEGMENTATION:
        if(mShowValid)
        {
            if(mShowValidInColor){
                return SCC_SEGMENTATION_VALID;
            }
            return SCC_SEGMENTATION_VALID_BLACK;
        }

        if(shouldVolumeBeShownBroken())
        {
            if(mShowFilterInColor){
                return SCC_SEGMENTATION_BREAK_COLOR;
            }
            return SCC_SEGMENTATION_BREAK_BLACK;
        }

        return SCC_SEGMENTATION;
    }

    throw OmArgException("unknown objType");
}

void OmViewGroupState::SetToolBarManager(ToolBarManager* tbm)
{
    toolBarManager_ = tbm;
    cutting_->SetToolBarManager(tbm);
    splitting_->SetToolBarManager(tbm);
}

void OmViewGroupState::SetShowValidMode(bool mode, bool inColor)
{
    mShowValid = mode;
    mShowValidInColor = inColor;
    OmEvents::Redraw3d();
    OmEvents::Redraw2d();
}

void OmViewGroupState::SetHowNonSelectedSegmentsAreColoredInFilter(const bool inColor)
{
    mShowFilterInColor = inColor;
    OmEvents::Redraw2d();
}

bool OmViewGroupState::shouldVolumeBeShownBroken()
{
    return mShatter || (splitting_->ShowSplit());
}

void OmViewGroupState::setTool(const om::tool::mode tool){
    toolBarManager_->SetTool(tool);
}
