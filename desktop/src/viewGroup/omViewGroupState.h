#pragma once
#include "precomp.h"

#include "segment/colorizer/omSegmentColorizerTypes.h"
#include "system/omManageableObject.h"
#include "common/colors.h"
#include "gui/tools.hpp"

class QGLWidget;
class ChannelDataWrapper;
class InspectorProperties;
class MainWindow;
class OmColorizers;
class OmLandmarks;
class OmSegmentColorizer;
class OmJoiningSplitting;
class OmSegmentSelector;
class OmTileCoord;
class OmViewGroupView2dState;
class OmZoomLevel;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class ToolBarManager;
class ViewGroup;

class OmViewGroupState : public OmManageableObject {
 public:
  OmViewGroupState(MainWindow* mw);

  ~OmViewGroupState();

  ViewGroup& GetViewGroup() { return *viewGroup_; }

  // viewbox state
  inline OmViewGroupView2dState& View2dState() { return *view2dState_; }

  inline OmZoomLevel& ZoomLevel() { return *zoomLevel_; }

  void setBreakThreshold(float t) { mBreakThreshold = t; }
  float getBreakThreshold() { return mBreakThreshold; }

  void setDustThreshold(uint64_t t) { mDustThreshold = t; }
  uint64_t getDustThreshold() { return mDustThreshold; }

  void SetToolBarManager(ToolBarManager* tbm);
  ToolBarManager& GetToolBarManager();

  void SetShouldVolumeBeShownBroken(bool shouldVolumeBeShownBroken);

  void SetShowValidMode(bool mode, bool incolor);
  bool shouldVolumeBeShownBroken();

  OmJoiningSplitting& JoiningSplitting() { return *joiningSplitting_; }

  std::shared_ptr<OmSegmentSelector> GetOrCreateSelector(om::common::ID segmentationID,
      const std::string& comment);
  inline bool IsSelecting() { return static_cast<bool>(selector_); }
  void EndSelector();

  void SetHowNonSelectedSegmentsAreColoredInFilter(const bool);

  inline bool ShowNonSelectedSegmentsInColorInFilter() const {
    return mShowFilterInColor;
  }

  om::segment::coloring determineColorizationType(const om::common::ObjectType);

  std::shared_ptr<om::common::ColorARGB> ColorTile(uint32_t const* const,
                                                   const int tileDim,
                                                   const OmTileCoord&);

  ChannelDataWrapper Channel() const;
  SegmentationDataWrapper Segmentation() const;

  void BrightenSelected(const bool brigthen) { brightenSelected_ = brigthen; }

  inline OmLandmarks& Landmarks() { return *landmarks_; }

  inline const om::common::Color& getAnnotationColor() {
    return annotationColor_;
  }

  inline void setAnnotationColor(const om::common::Color& color) {
    annotationColor_ = color;
  }

  inline const std::string& getAnnotationString() { return annotationString_; }

  inline void setAnnotationString(const std::string& string) {
    annotationString_ = string;
  }

  inline const double& getAnnotationSize() { return annotationSize_; }

  inline void setAnnotationSize(const double& size) { annotationSize_ = size; }

  bool getAnnotationVisible();
  void setAnnotationVisible(bool visible);

#ifdef ZI_OS_MACOS
  inline QGLWidget* get3dContext() { return context3d_.get(); }
#endif
 private:
  std::unique_ptr<ViewGroup> viewGroup_;
  std::unique_ptr<OmViewGroupView2dState> view2dState_;
  std::unique_ptr<OmColorizers> colorizers_;
  std::unique_ptr<OmZoomLevel> zoomLevel_;
  std::unique_ptr<OmJoiningSplitting> joiningSplitting_;
  std::unique_ptr<OmLandmarks> landmarks_;
  std::shared_ptr<OmSegmentSelector> selector_;

  std::unique_ptr<ChannelDataWrapper> cdw_;
  std::unique_ptr<SegmentationDataWrapper> sdw_;

#ifdef ZI_OS_MACOS
  std::unique_ptr<QGLWidget> context3d_;
#endif

  float mBreakThreshold;
  uint64_t mDustThreshold;

  // toolbar stuff
  ToolBarManager* toolBarManager_;
  bool mShowValid;
  bool mShowValidInColor;
  bool mShowFilterInColor;

  bool brightenSelected_;

  // annotation stuff
  om::common::Color annotationColor_;
  std::string annotationString_;
  double annotationSize_;
};
