#include "common/logging.h"
#include "events/events.h"
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
#include "users/omUsers.h"

#ifdef ZI_OS_MACOS
#include <QGLWidget>
#endif

OmViewGroupState::OmViewGroupState(MainWindow* mainWindow)
    : OmManageableObject(),
      viewGroup_(new ViewGroup(mainWindow, *this)),
      view2dState_(new OmViewGroupView2dState()),
      colorizers_(new OmColorizers(*this)),
      zoomLevel_(new OmZoomLevel()),
      splitting_(new OmSplitting()),
      landmarks_(new OmLandmarks(mainWindow)),
      cdw_(new ChannelDataWrapper(1)),
      sdw_(new SegmentationDataWrapper(1))
#ifdef ZI_OS_MACOS
      ,
      context3d_(new QGLWidget())
#endif
      ,
      toolBarManager_(nullptr),
      brightenSelected_(true),
      annotationSize_(3) {
  mBreakThreshold = 0;
  mDustThreshold = 90;
  mShatter = false;
  mShowValid = false;
  mShowValidInColor = false;
  mShowFilterInColor = false;
}

OmViewGroupState::~OmViewGroupState() {}

std::shared_ptr<om::common::ColorARGB> OmViewGroupState::ColorTile(
    uint32_t const* const imageData, const int tileDim,
    const OmTileCoord& key) {
  return colorizers_->ColorTile(imageData, tileDim, key);
}

om::segment::coloring OmViewGroupState::determineColorizationType(
    const om::common::ObjectType objType) {

  switch (objType) {
    case om::common::CHANNEL:
      if (mShowValid) {
        if (mShowValidInColor) {
          return om::segment::coloring::FILTER_VALID;
        }
        return om::segment::coloring::FILTER_VALID_BLACK;
      }

      if (shouldVolumeBeShownBroken()) {
        return om::segment::coloring::FILTER_BREAK;
      }

      if (brightenSelected_) {
        if (mShowFilterInColor) {
          return om::segment::coloring::FILTER_COLOR_BRIGHTEN_SELECT;
        }

        return om::segment::coloring::FILTER_BLACK_BRIGHTEN_SELECT;
      }

      if (mShowFilterInColor) {
        return om::segment::coloring::FILTER_COLOR_DONT_BRIGHTEN_SELECT;
      }

      return om::segment::coloring::FILTER_BLACK_DONT_BRIGHTEN_SELECT;

    case om::common::SEGMENTATION:
      if (mShowValid) {
        if (mShowValidInColor) {
          return om::segment::coloring::SEGMENTATION_VALID;
        }
        return om::segment::coloring::SEGMENTATION_VALID_BLACK;
      }

      if (shouldVolumeBeShownBroken()) {
        if (mShowFilterInColor) {
          return om::segment::coloring::SEGMENTATION_BREAK_COLOR;
        }
        return om::segment::coloring::SEGMENTATION_BREAK_BLACK;
      }

      return om::segment::coloring::SEGMENTATION;
    default:
      break;
  }

  throw om::ArgException("unknown objType");
}

void OmViewGroupState::SetToolBarManager(ToolBarManager* tbm) {
  toolBarManager_ = tbm;
  splitting_->SetToolBarManager(tbm);
}

ToolBarManager& OmViewGroupState::GetToolBarManager() {
  return *toolBarManager_;
}

void OmViewGroupState::SetShowValidMode(bool mode, bool inColor) {
  mShowValid = mode;
  mShowValidInColor = inColor;
  om::event::Redraw3d();
  om::event::Redraw2d();
}

OmSegmentSelector& OmViewGroupState::Selector(om::common::ID segmentationID, const std::string& comment) {
  if (!IsSelecting()) {
    SegmentationDataWrapper sdw(segmentationID);
    selector_ = std::make_unique(sdw.GetSegmentation(), nullptr, comment);
  }
  return *selector_;
}

bool OmViewGroupState::IsSelecting() {
  return selector_;
}

void OmViewGroupState::EndSelecting() {
  selector_.reset();
}

void OmViewGroupState::SetHowNonSelectedSegmentsAreColoredInFilter(
    const bool inColor) {
  mShowFilterInColor = inColor;
  om::event::Redraw2d();
}

bool OmViewGroupState::shouldVolumeBeShownBroken() {
  return mShatter || splitting_->ShowSplit();
}

void OmViewGroupState::setTool(const om::tool::mode tool) {
  toolBarManager_->SetTool(tool);
}

SegmentationDataWrapper OmViewGroupState::Segmentation() const { return *sdw_; }

ChannelDataWrapper OmViewGroupState::Channel() const { return *cdw_; }

bool OmViewGroupState::getAnnotationVisible() {
  return OmProject::Globals().Users().UserSettings().getAnnotationVisible();
}

void OmViewGroupState::setAnnotationVisible(bool visible) {
  OmProject::Globals().Users().UserSettings().setAnnotationVisible(visible);
}
