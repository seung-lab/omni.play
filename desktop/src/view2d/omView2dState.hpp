#pragma once

#include "coordinates/normCoord.h"
#include "events/omEvents.h"
#include "system/cache/omCacheManager.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "tiles/cache/omTileCache.h"
#include "utility/segmentationDataWrapper.hpp"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "viewGroup/omZoomLevel.hpp"
#include "events/details/omViewEvent.h"
#include "volume/omMipVolume.h"
#include "view2d/view2dCoords.hpp"
#include "coordinates/screenCoord.h"

#include "vmmlib/vmmlib.h"
using namespace vmml;

#include <QSize>
#include <QMouseEvent>

class OmBrushSize;

/**
 * Encapsulate most of state needed by view2d
 *
 * a shallow copy will be made by OmTilePreFetcher
 *
 */

class OmView2dState {
 private:
  OmMipVolume* vol_;
  const om::common::ObjectType objType_;
  OmViewGroupState* const vgs_;
  OmZoomLevel* const zoomLevel_;

  const om::common::ViewType viewType_;
  const std::string name_;

  bool scribbling_;
  Vector2i mousePoint_;
  bool isLevelLocked_;

  boost::optional<om::screenCoord> mousePanStartingPt_;

  // (x,y) coordinates only (no depth); needed for Bresenham
  om::globalCoord lastDataPoint_;

  OmBrushSize* const brushSize_;

  bool overrideToolModeForPan_;

  om::common::SegID segIDforPainting_;

  om::view2dCoords coords_;

 public:
  OmView2dState(OmMipVolume* vol, OmViewGroupState* vgs,
                const om::common::ViewType viewType, const QSize& size,
                const std::string name)
      : vol_(vol),
        objType_(vol->getVolumeType()),
        vgs_(vgs),
        zoomLevel_(vgs->ZoomLevel()),
        viewType_(viewType),
        name_(name),
        scribbling_(false),
        mousePoint_(0, 0),
        isLevelLocked_(false),
        lastDataPoint_(0, 0, 0),
        brushSize_(OmStateManager::BrushSize()),
        overrideToolModeForPan_(false),
        segIDforPainting_(0),
        coords_(vgs, viewType) {
    coords_.setTotalViewport(size);
    zoomLevel_->Update(getMaxMipLevel());

    coords_.UpdateTransformationMatrices();
  }

  inline om::view2dCoords& Coords() { return coords_; }

  inline const om::view2dCoords& Coords() const { return coords_; }

  void Shift(const om::common::Direction dir) {
    const float numberOfSlicestoAdvance = 2 * om::math::pow2int(getMipLevel());
    om::globalCoord loc = Location();
    OmView2dConverters::ShiftPanDirection(loc, numberOfSlicestoAdvance, dir,
                                          viewType_);
    setLocation(loc);
    OmEvents::Redraw2d();
    coords_.UpdateTransformationMatrices();
  }

  inline Vector2f ComputePanDistance() const {
    Vector3f pan = coords_.GlobalToScreenMat().getTranslation();
    return OmView2dConverters::Get2PtsInPlane(pan, viewType_);
  }

  inline void SetViewSliceOnPan() {
    const Vector4i& viewport = coords_.getTotalViewport();
    om::globalCoord min = om::screenCoord(0, 0, this).toGlobalCoord();
    om::globalCoord max =
        om::screenCoord(viewport.width, viewport.height, this).toGlobalCoord();

    vgs_->View2dState()->SetViewSliceMax(viewType_, get2ptsInPlane(max));
    vgs_->View2dState()->SetViewSliceMin(viewType_, get2ptsInPlane(min));

    OmEvents::Redraw3d();
  }

  inline void ChangeViewCenter() {
    coords_.UpdateTransformationMatrices();
    SetViewSliceOnPan();
  }

  void SetIntialWindowState() {
    OmViewGroupView2dState* v2dstate = vgs_->View2dState();

    if (v2dstate->GetInitialized()) {
      OmTileCache::ClearAll();  // tile opengl contexts may have changed
      return;
    }

    ResetWindowState();
    v2dstate->SetInitialized();
  }

  void ResetWindowState() {
    static const om::normCoord midPoint(0.5, 0.5, 0.5, vol_);

    std::cout << vol_->Coords().GetDataDimensions() << std::endl;

    om::globalCoord loc = midPoint.toGlobalCoord();
    setLocation(loc);

    zoomLevel_->Reset(getMaxMipLevel());

    coords_.UpdateTransformationMatrices();

    OmTileCache::ClearAll();
    OmEvents::Redraw2d();
  }

  inline void MoveUpStackCloserToViewer(int steps = 1) {
    const int numberOfSlicestoAdvance =
        om::math::pow2int(getMipLevel()) *
        getViewTypeDepth(vol_->Coords().GetResolution()) * steps;
    const int depth = vgs_->View2dState()->GetScaledSliceDepth(viewType_);
    vgs_->View2dState()
        ->SetScaledSliceDepth(viewType_, depth + numberOfSlicestoAdvance);

    coords_.UpdateTransformationMatrices();
  }

  inline void MoveDownStackFartherFromViewer(int steps = 1) {
    const int numberOfSlicestoAdvance =
        om::math::pow2int(getMipLevel()) *
        getViewTypeDepth(vol_->Coords().GetResolution()) * steps;
    const int depth = vgs_->View2dState()->GetScaledSliceDepth(viewType_);
    vgs_->View2dState()
        ->SetScaledSliceDepth(viewType_, depth - numberOfSlicestoAdvance);

    coords_.UpdateTransformationMatrices();
  }

  // mouse movement
  inline void DoMousePan(const om::screenCoord& cursorLocation) {
    if (!mousePanStartingPt_) {
      return;
    }
    const om::globalCoord difference =
        mousePanStartingPt_->toGlobalCoord() - cursorLocation.toGlobalCoord();

    setLocation(Location() + difference);

    OmEvents::ViewCenterChanged();

    if (difference != Vector3f::ZERO) {
      mousePanStartingPt_.reset(cursorLocation);
    }
  }

  // name
  inline const std::string& getName() const { return name_; }

  // viewtype
  inline om::common::ViewType getViewType() const { return viewType_; }

  // volume
  inline OmMipVolume* getVol() const { return vol_; }

  // view group state
  inline OmViewGroupState* getViewGroupState() const { return vgs_; }

  // scribbling
  inline bool getScribbling() { return scribbling_; }
  void setScribbling(const bool s) { scribbling_ = s; }

  // zoom and mip level
  inline float getZoomScale() const { return zoomLevel_->GetZoomScale(); }
  inline int getMipLevel() const { return zoomLevel_->GetMipLevel(); }
  inline int getMaxMipLevel() const { return vol_->Coords().GetRootMipLevel(); }

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

  template <typename T> inline T getViewTypeDepth(const Vector3<T>& vec) const {
    return OmView2dConverters::GetViewTypeDepth(vec, viewType_);
  }

  inline float getViewTypeDepth(const om::globalCoord& vec) const {
    return OmView2dConverters::GetViewTypeDepth(vec, viewType_);
  }

  template <typename T>
  inline void setViewTypeDepth(Vector3<T>& vec, const T& val) const {
    OmView2dConverters::SetViewTypeDepth(vec, val, viewType_);
  }

  inline void setViewTypeDepth(om::globalCoord& vec, const float val) const {
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

  void SetMousePanStartingPt(const om::screenCoord& vec) {
    mousePanStartingPt_ = vec;
  }

  // last data point--coupled w/ click point?
  inline const om::globalCoord& GetLastDataPoint() const {
    return lastDataPoint_;
  }
  void SetLastDataPoint(const om::globalCoord& coord) {
    lastDataPoint_ = coord;
  }

  // whether overall view2d widget is displaying a channel or a segmentation
  inline om::common::ObjectType getObjectType() const { return objType_; }

  inline om::common::ID GetSegmentationID() const {
    return vgs_->Segmentation().GetID();
  }

  inline SegmentationDataWrapper GetSDW() const { return vgs_->Segmentation(); }

  inline OmZoomLevel* ZoomLevel() { return zoomLevel_; }

  inline om::globalCoord Location() {
    return vgs_->View2dState()->GetScaledSliceDepth();
  }

  inline void setLocation(om::globalCoord loc) {
    vgs_->View2dState()->SetScaledSliceDepth(loc);
    coords_.UpdateTransformationMatrices();
  }

  inline bool OverrideToolModeForPan() { return overrideToolModeForPan_; }

  inline void OverrideToolModeForPan(const bool b) {
    overrideToolModeForPan_ = b;
  }

  inline om::common::SegID GetSegIDForPainting() { return segIDforPainting_; }

  inline void SetSegIDForPainting(const om::common::SegID segID) {
    segIDforPainting_ = segID;
  }
};
