#pragma once
#include "precomp.h"

#include "events/events.h"
#include "system/cache/omCacheManager.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "tiles/cache/omTileCache.h"
#include "utility/segmentationDataWrapper.hpp"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "viewGroup/omZoomLevel.hpp"
#include "events/events.h"
#include "volume/omMipVolume.h"

#include "vmmlib/vmmlib.h"
using namespace vmml;

class OmBrushSize;

/**
 * Encapsulate most of state needed by view2d
 *
 * a shallow copy will be made by OmTilePreFetcher
 *
 */

class OmView2dState {
 public:
  OmView2dState(OmMipVolume& vol, OmViewGroupState& vgs,
                const om::common::ViewType viewType, const QSize& size,
                const std::string name)
      : vol_(vol),
        objType_(vol.getVolumeType()),
        vgs_(vgs),
        zoomLevel_(vgs.ZoomLevel()),
        viewType_(viewType),
        name_(name),
        scribbling_(false),
        mousePoint_(0, 0),
        isLevelLocked_(false),
        lastDataPoint_(0, 0, 0),
        brushSize_(OmStateManager::BrushSize()),
        overrideToolModeForPan_(false),
        segIDforPainting_(0),
        coords_(viewType) {
    coords_.set_totalViewport(Vector4i(0, 0, size.width(), size.height()));
    zoomLevel_.Update(getMaxMipLevel());

    UpdateTransformationMatrices();
  }

  inline om::coords::ScreenSystem& Coords() { return coords_; }

  inline const om::coords::ScreenSystem& Coords() const { return coords_; }

  inline void UpdateTransformationMatrices() {
    coords_.set_zoomScale(zoomLevel_.GetZoomScale());
    coords_.set_location(Location());
    coords_.UpdateTransformationMatrices();
  }

  void Shift(const om::common::Direction dir) {
    const float numberOfSlicestoAdvance = 2 * om::math::pow2int(getMipLevel());
    om::coords::Global loc = Location();
    OmView2dConverters::ShiftPanDirection(loc, numberOfSlicestoAdvance, dir,
                                          viewType_);
    setLocation(loc);
    om::event::Redraw2d();
    UpdateTransformationMatrices();
  }

  inline Vector2f ComputePanDistance() const {
    Vector3f pan = coords_.globalToScreenMat().getTranslation();
    return OmView2dConverters::Get2PtsInPlane(pan, viewType_);
  }

  inline void SetViewSliceOnPan() {
    const Vector4i& viewport = coords_.totalViewport();
    om::coords::Global min = om::coords::Screen(0, 0, Coords()).ToGlobal();
    om::coords::Global max = om::coords::Screen(viewport.width, viewport.height,
                                                Coords()).ToGlobal();

    vgs_.View2dState().SetViewSliceMax(viewType_, get2ptsInPlane(max));
    vgs_.View2dState().SetViewSliceMin(viewType_, get2ptsInPlane(min));

    om::event::Redraw3d();
  }

  inline void ChangeViewCenter() {
    UpdateTransformationMatrices();
    SetViewSliceOnPan();
  }

  void SetIntialWindowState() {
    OmViewGroupView2dState& v2dstate = vgs_.View2dState();

    if (v2dstate.GetInitialized()) {
      OmTileCache::ClearAll();  // tile opengl contexts may have changed
      return;
    }

    ResetWindowState();
    v2dstate.SetInitialized();
  }

  void ResetWindowState() {
    static const om::coords::Norm midPoint(0.5, 0.5, 0.5, vol_.Coords());

    log_infos << vol_.Coords().DataDimensions() << std::endl;

    om::coords::Global loc = midPoint.ToGlobal();
    setLocation(loc);

    zoomLevel_.Reset(getMaxMipLevel());

    UpdateTransformationMatrices();

    OmTileCache::ClearAll();
    om::event::Redraw2d();
  }

  inline void MoveUpStackCloserToViewer(int steps = 1) {
    const int numberOfSlicestoAdvance =
        om::math::pow2int(getMipLevel()) *
        getViewTypeDepth(vol_.Coords().Resolution()) * steps;
    const int depth = vgs_.View2dState().GetScaledSliceDepth(viewType_);
    vgs_.View2dState().SetScaledSliceDepth(viewType_,
                                           depth + numberOfSlicestoAdvance);

    UpdateTransformationMatrices();
  }

  inline void MoveDownStackFartherFromViewer(int steps = 1) {
    const int numberOfSlicestoAdvance =
        om::math::pow2int(getMipLevel()) *
        getViewTypeDepth(vol_.Coords().Resolution()) * steps;
    const int depth = vgs_.View2dState().GetScaledSliceDepth(viewType_);
    vgs_.View2dState().SetScaledSliceDepth(viewType_,
                                           depth - numberOfSlicestoAdvance);

    UpdateTransformationMatrices();
  }

  // mouse movement
  inline void DoMousePan(const om::coords::Screen& cursorLocation) {
    if (!mousePanStartingPt_) {
      return;
    }
    const om::coords::Global difference =
        mousePanStartingPt_->ToGlobal() - cursorLocation.ToGlobal();

    setLocation(Location() + difference);

    om::event::ViewCenterChanged();

    if (difference != Vector3f::ZERO) {
      mousePanStartingPt_.reset(cursorLocation);
    }
  }

  // name
  inline const std::string& getName() const { return name_; }

  // viewtype
  inline om::common::ViewType getViewType() const { return viewType_; }

  // volume
  inline OmMipVolume& getVol() const { return vol_; }

  // view group state
  inline OmViewGroupState& getViewGroupState() const { return vgs_; }

  // scribbling
  inline bool getScribbling() { return scribbling_; }
  void setScribbling(const bool s) { scribbling_ = s; }

  // zoom and mip level
  inline float getZoomScale() const { return zoomLevel_.GetZoomScale(); }
  inline int getMipLevel() const { return zoomLevel_.GetMipLevel(); }
  inline int getMaxMipLevel() const { return vol_.Coords().RootMipLevel(); }

  // depth-related computation helpers
  template <typename T>
  inline Vector3<T> makeViewTypeVector3(const Vector3<T>& vec) const {
    return makeViewTypeVector3(vec.x, vec.y, vec.z);
  }

  template <typename T>
  inline Vector3<T> makeViewTypeVector3(const T& x, const T& y,
                                        const T& z) const {
    return OmView2dConverters::MakeViewTypeVector3(x, y, z, viewType_);
  }

  template <typename T>
  inline Vector2<T> get2ptsInPlane(const Vector3<T>& vec) {
    return OmView2dConverters::Get2PtsInPlane<T>(vec, viewType_);
  }

  template <typename T>
  inline T getViewTypeDepth(const Vector3<T>& vec) const {
    return OmView2dConverters::GetViewTypeDepth(vec, viewType_);
  }

  inline float getViewTypeDepth(const om::coords::Global& vec) const {
    return OmView2dConverters::GetViewTypeDepth(vec, viewType_);
  }

  template <typename T>
  inline void setViewTypeDepth(Vector3<T>& vec, const T& val) const {
    OmView2dConverters::SetViewTypeDepth(vec, val, viewType_);
  }

  inline void setViewTypeDepth(om::coords::Global& vec, const float val) const {
    OmView2dConverters::SetViewTypeDepth(vec, val, viewType_);
  }

  template <typename T>
  inline Vector3<T> scaleViewType(const T& x, const T& y,
                                  const Vector3<T>& scale) const {
    return OmView2dConverters::ScaleViewType(x, y, scale, viewType_);
  }

  template <typename T>
  inline Vector3<T> scaleViewType(const Vector2<T>& vec,
                                  const Vector3<T>& scale) const {
    return OmView2dConverters::ScaleViewType(vec, scale, viewType_);
  }

  // brush size
  inline OmBrushSize* getBrushSize() { return brushSize_; }

  // mouse point
  inline const Vector2i& GetMousePoint() const { return mousePoint_; }
  inline void SetMousePoint(QMouseEvent* event) {
    mousePoint_ = Vector2i(event->x(), event->y());
  }
  inline void SetMousePoint(const int x, const int y) {
    mousePoint_ = Vector2i(x, y);
  }

  // mip level lock
  inline bool IsLevelLocked() const { return isLevelLocked_; }
  void ToggleLevelLock() { isLevelLocked_ = !isLevelLocked_; }

  void SetMousePanStartingPt(const om::coords::Screen& vec) {
    mousePanStartingPt_ = vec;
  }

  // last data point--coupled w/ click point?
  inline const om::coords::Global& GetLastDataPoint() const {
    return lastDataPoint_;
  }
  void SetLastDataPoint(const om::coords::Global& coord) {
    lastDataPoint_ = coord;
  }

  // whether overall view2d widget is displaying a channel or a segmentation
  inline om::common::ObjectType getObjectType() const { return objType_; }

  inline om::common::ID GetSegmentationID() const {
    return vgs_.Segmentation().id();
  }

  inline SegmentationDataWrapper GetSDW() const { return vgs_.Segmentation(); }

  inline OmZoomLevel& ZoomLevel() { return zoomLevel_; }

  inline om::coords::Global Location() {
    return vgs_.View2dState().GetScaledSliceDepth();
  }

  inline void setLocation(om::coords::Global loc) {
    vgs_.View2dState().SetScaledSliceDepth(loc);
    UpdateTransformationMatrices();
  }

  inline bool OverrideToolModeForPan() { return overrideToolModeForPan_; }

  inline void OverrideToolModeForPan(const bool b) {
    overrideToolModeForPan_ = b;
  }

  inline om::common::SegID GetSegIDForPainting() { return segIDforPainting_; }

  inline void SetSegIDForPainting(const om::common::SegID segID) {
    segIDforPainting_ = segID;
  }

 private:
  OmMipVolume& vol_;
  const om::common::ObjectType objType_;
  OmViewGroupState& vgs_;
  OmZoomLevel& zoomLevel_;

  const om::common::ViewType viewType_;
  const std::string name_;

  bool scribbling_;
  Vector2i mousePoint_;
  bool isLevelLocked_;

  boost::optional<om::coords::Screen> mousePanStartingPt_;

  // (x,y) coordinates only (no depth); needed for Bresenham
  om::coords::Global lastDataPoint_;

  OmBrushSize* const brushSize_;

  bool overrideToolModeForPan_;

  om::common::SegID segIDforPainting_;

  om::coords::ScreenSystem coords_;
};
