#include "common/omDebug.h"
#include "events/omEvents.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/toolbars/toolbarManager.h"
#include "gui/viewGroup/viewGroup.h"
#include "landmarks/omLandmarks.hpp"
#include "segment/colorizer/omSegmentColorizer.h"
#include "segment/omSegment.h"
#include "system/cache/omCacheManager.h"
#include "system/omStateManager.h"
#include "tiles/omTileCoord.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omColorizers.hpp"
#include "viewGroup/omSplitting.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "viewGroup/omZoomLevel.hpp"
#include "view2d/omView2dState.hpp"

#ifdef ZI_OS_MACOS
#include <QGLWidget>
#endif

OmViewGroupState::OmViewGroupState(MainWindow* mainWindow)
    : OmManageableObject()
    , viewGroup_(new ViewGroup(mainWindow, this))
    , view2dState_(new OmViewGroupView2dState())
    , colorizers_(new om::common::Colorizers(this))
    , zoomLevel_(new OmZoomLevel())
    , splitting_(new OmSplitting())
    , landmarks_(new OmLandmarks(mainWindow))
    , cdw_(new ChannelDataWrapper(1))
    , sdw_(new SegmentationDataWrapper(1))
#ifdef ZI_OS_MACOS
    , context3d_(new QGLWidget())
#endif
    , toolBarManager_(NULL)
    , brightenSelected_(true)
    , annotationVisible_(true)
    , annotationSize_(3)
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

OmPooledTile<OmColorARGB>* OmViewGroupState::ColorTile(uint32_t const*const imageData,
                                                       const int tileDim,
                                                       const OmTileCoord& key)
{
    return colorizers_->ColorTile(imageData, tileDim, key);
}

om::common::SegmentColorCacheType
OmViewGroupState::determineColorizationType(const om::common::ObjectType objType)
{
    switch(objType){
    case CHANNEL:
        if(mShowValid)
        {
            if(mShowValidInColor){
                return om::common::SCC_FILTER_VALID;
            }
            return om::common::om::common::SCC_FILTER_VALID_BLACK;
        }

        if(shouldVolumeBeShownBroken()){
            return om::common::SCC_FILTER_BREAK;
        }

        if(brightenSelected_)
        {
            if(mShowFilterInColor){
                return om::common::SCC_FILTER_COLOR_BRIGHTEN_SELECT;
            }

            return om::common::SCC_FILTER_BLACK_BRIGHTEN_SELECT;
        }

        if(mShowFilterInColor){
            return om::common::SCC_FILTER_COLOR_DONT_BRIGHTEN_SELECT;
        }

        return om::common::SCC_FILTER_BLACK_DONT_BRIGHTEN_SELECT;

    case SEGMENTATION:
        if(mShowValid)
        {
            if(mShowValidInColor){
                return om::common::om::common::SCC_SEGMENTATION_VALID;
            }
            return om::common::om::common::om::common::SCC_SEGMENTATION_VALID_BLACK;
        }

        if(shouldVolumeBeShownBroken())
        {
            if(mShowFilterInColor){
                return om::common::om::common::SCC_SEGMENTATION_BREAK_COLOR;
            }
            return om::common::om::common::SCC_SEGMENTATION_BREAK_BLACK;
        }

        return om::common::SCC_SEGMENTATION;
    default:
    	break;
    }

    throw om::ArgException("unknown objType");
}

void OmViewGroupState::SetToolBarManager(ToolBarManager* tbm)
{
    toolBarManager_ = tbm;
    splitting_->SetToolBarManager(tbm);
}

ToolBarManager* OmViewGroupState::GetToolBarManager() {
    return toolBarManager_;
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
    return mShatter || splitting_->ShowSplit();
}

void OmViewGroupState::setTool(const om::tool::mode tool){
    toolBarManager_->SetTool(tool);
}

SegmentationDataWrapper OmViewGroupState::Segmentation() const {
    return *sdw_;
}

ChannelDataWrapper OmViewGroupState::Channel() const{
    return *cdw_;
}
