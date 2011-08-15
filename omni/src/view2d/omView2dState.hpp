#pragma once

#include "common/omDebug.h"
#include "events/omEvents.h"
#include "system/cache/omCacheManager.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "tiles/cache/omTileCache.h"
#include "utility/segmentationDataWrapper.hpp"
#include "view2d/omView2dConverters.hpp"
#include "view2d/omView2dVolLocation.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omZoomLevel.hpp"
#include "volume/omMipVolume.h"

#include <QSize>
#include <QMouseEvent>

class OmBrushSize;

/**
 * Encapsulate most of state needed by view2d
 *
 * a shallow copy will be made by OmTilePreFetcher
 *
 */

class OmView2dState{
public:
    OmView2dState(OmMipVolume* vol,
                  OmViewGroupState* vgs,
                  const ViewType viewType,
                  const QSize& size,
                  const std::string name)
        : vol_(vol)
        , objType_(vol->getVolumeType())
        , vgs_(vgs)
        , zoomLevel_(vgs->ZoomLevel())
        , viewType_(viewType)
        , name_(name)
        , scribbling_(false)
        , mousePoint_(0,0)
        , isLevelLocked_(false)
        , lastDataPoint_(0,0)
        , location_(om::make_shared<OmView2dVolLocation>(vol, vgs, viewType))
        , brushSize_(OmStateManager::BrushSize())
        , overrideToolModeForPan_(false)
        , segIDforPainting_(0)
    {
        setTotalViewport(size);
        zoomLevel_->Update(getMaxMipLevel());
    }

    // coord convertors

private:
    inline Vector2f screenToPanShift(const Vector2f screenshift) const
    {
        const Vector2f stretch= vol_->Coords().GetStretchValues(viewType_);
        const float zoomScale = getZoomScale();

        return Vector2f(screenshift.x/zoomScale/stretch.x,
                        screenshift.y/zoomScale/stretch.y);
    }

public:
    inline DataCoord ScreenToDataCoord(const ScreenCoord& screenc) const
    {
        const Vector2f panDistance = ComputePanDistance();
        const float factor = om::math::pow2int(getMipLevel());

        const Vector2f unscaleNorm =
            (screenToPanShift(screenc) - panDistance) * factor;

        const Vector3f dataScale = vol_->Coords().GetDataDimensions();

        const NormCoord normc = scaleViewType(unscaleNorm, dataScale);
        Vector3i result = vol_->Coords().NormToDataCoord(normc);

        const int depth = location_->DataDepth();

        setViewTypeDepth(result, depth);

//         std::cout << "ScreenToDataCoord: screenc: " << screenc
//                   << "\n\tpanDistance: " << panDistance
//                   << "\n\t factor: " << factor
//                   << "\n\t screenToPanShift(screenc): " << screenToPanShift(screenc)
//                   << "\n\t unscaleNorm: " << unscaleNorm
//                   << "\n\t dataScale: " << dataScale
//                   << "\n\t normc: " << normc
//                   << "\n\t result: " << result
//                   << "\n\t depth: " << depth
//                   << "\n";

        return result;
    }

private:
    inline ScreenCoord dataToScreenCoord(const DataCoord& inc) const
    {
        const NormCoord normCoord = vol_->Coords().DataToNormCoord(inc);
        const Vector3f scale = vol_->Coords().GetDataDimensions();
        const Vector3f datac = normCoord * scale;

        const Vector2f panDistance = ComputePanDistance();
        const Vector2f stretch = vol_->Coords().GetStretchValues(viewType_);
        const float factor = om::math::pow2int(getMipLevel());
        const float zoomScale = getZoomScale();

        switch(viewType_){
        case XY_VIEW:
            return ScreenCoord((int)((float)(datac.x/factor+panDistance.x)*zoomScale*stretch.x),
                               (int)((float)(datac.y/factor+panDistance.y)*zoomScale*stretch.y));
        case XZ_VIEW:
            return ScreenCoord((int)((float)(datac.x/factor+panDistance.x)*zoomScale*stretch.x),
                               (int)((float)(datac.z/factor+panDistance.y)*zoomScale*stretch.y));
        case ZY_VIEW:
            return ScreenCoord((int)((float)(datac.z/factor+panDistance.x)*zoomScale*stretch.x),
                               (int)((float)(datac.y/factor+panDistance.y)*zoomScale*stretch.y));
        };

        throw OmArgException("invalid viewType");
    }

public:
    void Shift(const om::Direction dir)
    {
        Vector3f curLocation = location_->DataLocation();
        const float numberOfSlicestoAdvance = 2 * om::math::pow2int(getMipLevel());
        OmView2dConverters::ShiftPanDirection(curLocation, numberOfSlicestoAdvance, dir, viewType_);
        location_->SetDataLocation(curLocation);
        OmEvents::Redraw2d();
    }

    inline Vector2f ComputePanDistance() const
    {
        const Vector2f stretch = vol_->Coords().GetStretchValues(viewType_);
        const float factor = om::math::pow2int(getMipLevel());

        const Vector3f curLocation = location_->DataLocation();

        const float zoomScale = getZoomScale();
        const float panx = (totalViewport_.width/2.0) / zoomScale;
        const float pany = (totalViewport_.height/2.0) / zoomScale;

        Vector2f oldPts = OmView2dConverters::Get2PtsInPlane(curLocation, viewType_);
        oldPts /= factor;

        const Vector2f ret = Vector2f(panx, pany) - oldPts;
        // std::cout << "pan: " << ret << "\n";
        // std::cout << "stretch: " << stretch << "\n";
        return ret;
    }

    inline void SetViewSliceOnPan()
    {
        const Vector2f translateVector = ComputePanDistance();

        const float pl = om::math::pow2int(getMipLevel());
        const float zoomFactor = getZoomScale();

        const DataCoord minDCoord(totalViewport_.lowerLeftX - translateVector.x * pl,
                                  totalViewport_.lowerLeftY - translateVector.y * pl,
                                  0);

        const DataCoord maxDCoord(totalViewport_.width / zoomFactor * pl - translateVector.x * pl,
                                  totalViewport_.height / zoomFactor * pl - translateVector.y * pl,
                                  0);

        location_->SetDataMinAndMax(minDCoord, maxDCoord);

        OmEvents::Redraw3d();
    }

    inline void ChangeViewCenter()
    {
        const Vector3f depth = location_->ScaledLocation();

        const ScreenCoord crossCoord = dataToScreenCoord(depth);

        const ScreenCoord centerCoord(totalViewport_.width/2,
                                      totalViewport_.height/2);

        const Vector2f currentPan = ComputePanDistance();
        const Vector2f newPan =  currentPan + screenToPanShift(centerCoord - crossCoord);

        SetViewSliceOnPan();
    }

    void SetIntialWindowState()
    {
        OmViewGroupView2dState* v2dstate = vgs_->View2dState();

        if(v2dstate->GetInitialized())
        {
            OmTileCache::ClearAll(); // tile opengl contexts may have changed
            return;
        }

        ResetWindowState();
        v2dstate->SetInitialized();
    }

    void ResetWindowState()
    {
        static const NormCoord midPoint(0.5, 0.5, 0.5);
        const DataCoord depth = vol_->Coords().NormToDataCoord(midPoint);

        location_->SetDataLocation(depth);

        zoomLevel_->Reset(getMaxMipLevel());

        OmTileCache::ClearAll();
        OmEvents::Redraw2d();
    }

    inline void MoveUpStackCloserToViewer()
    {
        const int numberOfSlicestoAdvance = om::math::pow2int(getMipLevel());
        const int depth = location_->DataDepth();
        location_->SetDataDepth(depth + numberOfSlicestoAdvance);
    }

    inline void MoveDownStackFartherFromViewer()
    {
        const int numberOfSlicestoAdvance = om::math::pow2int(getMipLevel());
        const int depth = location_->DataDepth();
        location_->SetDataDepth(depth - numberOfSlicestoAdvance);
    }

    // mouse movement
    inline void DoMousePan(const Vector2i& cursorLocation)
    {
        if(!mousePanStartingPt_){
            return;
        }
	const DataCoord difference = ScreenToDataCoord(*mousePanStartingPt_) - ScreenToDataCoord(cursorLocation);

        const DataCoord oldLocation = location_->DataLocation();
        const DataCoord newLocation = oldLocation + difference;

        location_->SetDataLocation(newLocation);

        OmEvents::ViewCenterChanged();

//         std::cout << "cursorLocation: " << cursorLocation
//                   << "\n\t mousePanStartingPt_: " << *mousePanStartingPt_
//                   << "\n\t difference: " << difference
//                   << "\n\t oldLocation: " << oldLocation
//                   << "\n\t newLocation: " << newLocation
//                   << "\n";

        if(oldLocation != newLocation){
            mousePanStartingPt_ = cursorLocation;
        }
    }

    // name
    inline const std::string& getName() const {
        return name_;
    }

    // viewtype
    inline ViewType getViewType() const {
        return viewType_;
    }

    // viewport
    inline const Vector4i& getTotalViewport() const {
        return totalViewport_;
    }
    void setTotalViewport(const QSize& size){
        totalViewport_ = Vector4i(0, 0, size.width(), size.height());
    }
    void setTotalViewport(const int width, const int height){
        totalViewport_ = Vector4i(0, 0, width, height);
    }

    // volume
    inline OmMipVolume* getVol() const {
        return vol_;
    }
    void setVol(OmMipVolume* vol){
        vol_ = vol;
    }

    // view group state
    inline OmViewGroupState* getViewGroupState() const {
        return vgs_;
    }

    // scribbling
    inline bool getScribbling(){
        return scribbling_;
    }
    void setScribbling(const bool s){
        scribbling_ = s;
    }

    // zoom and mip level
    inline float getZoomScale() const {
        return zoomLevel_->GetZoomScale();
    }
    inline int getMipLevel() const {
        return zoomLevel_->GetMipLevel();
    }
    inline int getMaxMipLevel() const {
        return vol_->Coords().GetRootMipLevel();
    }

    // depth-related computation helpers
    template <typename T>
    inline Vector3<T> makeViewTypeVector3(const Vector3<T>& vec) const {
        return makeViewTypeVector3(vec.x, vec.y, vec.z);
    }

    template <typename T>
    inline Vector3<T> makeViewTypeVector3(const T& x, const T& y,
                                          const T& z) const {
        return OmView2dConverters::MakeViewTypeVector3(x,y,z,viewType_);
    }

    template <typename T>
    inline T getViewTypeDepth(const Vector3<T>& vec) const {
        return OmView2dConverters::GetViewTypeDepth(vec, viewType_);
    }

    template <typename T>
    inline void setViewTypeDepth(Vector3<T>& vec, const T& val) const {
        OmView2dConverters::SetViewTypeDepth(vec, val, viewType_);
    }

    template <typename T>
    inline Vector3<T> scaleViewType(const T& x, const T& y,
                                    const Vector3<T>& scale) const
    {
        return OmView2dConverters::ScaleViewType(x, y, scale, viewType_);
    }

    template <typename T>
    inline Vector3<T> scaleViewType(const Vector2<T>& vec,
                                    const Vector3<T>& scale) const
    {
        return OmView2dConverters::ScaleViewType(vec, scale, viewType_);
    }

    // for tile coord creation
    DataCoord computeTileDataCoord(const DataCoord& dataCoord) const
    {
        const DataBbox box = vol_->Coords().DataToDataBBox(dataCoord, 0);
        const int depth = getViewTypeDepth(dataCoord);

        Vector3i bcoord = box.getMin();
        setViewTypeDepth(bcoord, depth);

        return bcoord;
    }

    // brush size
    inline OmBrushSize* getBrushSize(){
        return brushSize_;
    }

    // mouse point
    inline const Vector2i& GetMousePoint() const {
        return mousePoint_;
    }
    inline void SetMousePoint(QMouseEvent* event){
        mousePoint_ = Vector2i(event->x(), event->y());
    }
    inline void SetMousePoint(const int x, const int y){
        mousePoint_ = Vector2i(x,y);
    }

    // mip level lock
    inline bool IsLevelLocked() const {
        return isLevelLocked_;
    }
    void ToggleLevelLock(){
        isLevelLocked_ = !isLevelLocked_;
    }

    void SetMousePanStartingPt(const Vector2f& vec){
        mousePanStartingPt_ = vec;
    }

    // mouse click coord
    inline DataCoord ComputeMouseClickPointDataCoord(QMouseEvent* event) const {
        return ScreenToDataCoord(Vector2f(event->x(), event->y()));
    }

    // last data point--coupled w/ click point?
    inline const DataCoord& GetLastDataPoint() const {
        return lastDataPoint_;
    }
    void SetLastDataPoint(const DataCoord& coord){
        lastDataPoint_ = coord;
    }

    // whether overall view2d widget is displaying a channel or a segmentation
    inline ObjectType getObjectType() const {
        return objType_;
    }

    // TODO: get from viewgroup state (purcaro)
    inline OmID GetSegmentationID() const {
        return 1;
    }

    // TODO: get from viewgroup state (purcaro)
    inline SegmentationDataWrapper GetSDW() const {
        return SegmentationDataWrapper(GetSegmentationID());
    }

    inline OmZoomLevel* ZoomLevel() {
        return zoomLevel_;
    }

    inline OmView2dVolLocation* Location(){
        return location_.get();
    }

    inline bool OverrideToolModeForPan(){
        return overrideToolModeForPan_;
    }

    inline void OverrideToolModeForPan(const bool b){
        overrideToolModeForPan_ = b;
    }

    inline OmSegID GetSegIDForPainting(){
        return segIDforPainting_;
    }

    inline void SetSegIDForPainting(const OmSegID segID){
        segIDforPainting_ = segID;
    }

private:
    OmMipVolume* vol_;
    const ObjectType objType_;
    OmViewGroupState *const vgs_;
    OmZoomLevel *const zoomLevel_;

    const ViewType viewType_;
    const std::string name_;

    Vector4i totalViewport_; //lower left x, lower left y, width, height
    bool scribbling_;
    Vector2i mousePoint_;
    bool isLevelLocked_;

    boost::optional<Vector2f> mousePanStartingPt_;

    // (x,y) coordinates only (no depth); needed for Bresenham
    DataCoord lastDataPoint_;

    om::shared_ptr<OmView2dVolLocation> location_;

    OmBrushSize *const brushSize_;

    bool overrideToolModeForPan_;

    OmSegID segIDforPainting_;
};

