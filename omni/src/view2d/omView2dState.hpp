#ifndef OM_VIEW2D_STATE_HPP
#define OM_VIEW2D_STATE_HPP

#include "common/omDebug.h"
#include "system/cache/omCacheManager.h"
#include "system/omEvents.h"
#include "system/omLocalPreferences.hpp"
#include "tiles/cache/omTileCache.h"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omZoomLevel.hpp"
#include "volume/omMipVolume.h"

#include <QSize>
#include <QMouseEvent>

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
		, viewType_(viewType)
		, name_(name)
		, scribbling_(false)
		, mousePoint_(0,0)
		, isLevelLocked_(false)
		, clickPoint_(0,0)
		, cameraMoving_(false)
		, lastDataPoint_(0,0)
	{
		setTotalViewport(size);
		ZoomLevel()->Update(getMaxMipLevel());
	}

	// coord convertors
	inline Vector2f ScreenToPanShift(const Vector2i& screenshift) const
	{
		const Vector2f stretch= vol_->Coords().GetStretchValues(viewType_);
		const float zoomScale = getZoomScale();
		const float panx = screenshift.x/zoomScale/stretch.x;
		const float pany = screenshift.y/zoomScale/stretch.y;
		return Vector2f(panx, pany);
	}

	inline DataCoord ScreenToDataCoord(const ScreenCoord& screenc) const
	{
		const Vector2f mPanDistance = ComputePanDistance();
		const Vector2f stretch = vol_->Coords().GetStretchValues(viewType_);

		const Vector3f dataScale = vol_->Coords().GetDataDimensions();
		const float factor = om::pow2int(getMipLevel());
		const float zoomScale = getZoomScale();

		const float unscaleNormX = ((float)screenc.x/zoomScale/stretch.x-mPanDistance.x)*factor;
		const float unscaleNormY = ((float)screenc.y/zoomScale/stretch.y-mPanDistance.y)*factor;

		const NormCoord normc = scaleViewType(unscaleNormX, unscaleNormY,
											  dataScale);
		Vector3i result = vol_->Coords().NormToDataCoord(normc);

		const int depth = getSliceDepth() / GetResOfDataSlice();
		setViewTypeDepth(result, depth);

		return result;
	}

	inline ScreenCoord DataToScreenCoord(const DataCoord& inc) const
	{
		const NormCoord normCoord = vol_->Coords().DataToNormCoord(inc);
		const Vector3f scale = vol_->Coords().GetDataDimensions();
		const Vector3f datac = normCoord * scale;

		const Vector2f mPanDistance = ComputePanDistance();
		const Vector2f stretch = vol_->Coords().GetStretchValues(viewType_);
		const float factor = om::pow2int(getMipLevel());
		const float zoomScale = getZoomScale();

		switch(viewType_){
		case XY_VIEW:
			return ScreenCoord((int)((float)(datac.x/factor+mPanDistance.x)*zoomScale*stretch.x),
							   (int)((float)(datac.y/factor+mPanDistance.y)*zoomScale*stretch.y));
		case XZ_VIEW:
			return ScreenCoord((int)((float)(datac.x/factor+mPanDistance.x)*zoomScale*stretch.x),
							   (int)((float)(datac.z/factor+mPanDistance.y)*zoomScale*stretch.y));
		case YZ_VIEW:
			return ScreenCoord((int)((float)(datac.z/factor+mPanDistance.x)*zoomScale*stretch.x),
							   (int)((float)(datac.y/factor+mPanDistance.y)*zoomScale*stretch.y));
		};

		throw OmArgException("invalid viewType");
	}

	// returns resolution scale factor for current viewType's depth
	inline float GetResOfDataSlice() const
	{
		const Vector3f& res = vol_->Coords().GetDataResolution();
		return getViewTypeDepth(res);
	}

	inline int GetDepthToDataSlice() const
	{
		const DataCoord depthCoord = vgs_->GetViewDepthCoord();
		return getViewTypeDepth(depthCoord);
	}

	inline void SetDataSliceToDepth(const int slice) const
	{
		setSliceDepth(slice);
	}

	inline Vector2f ComputePanDistance() const
	{
		const Vector2f stretch = vol_->Coords().GetStretchValues(viewType_);
		const float factor = om::pow2int(getMipLevel());

		const DataCoord mydataCoord = getViewSliceDepthData();

		const float zoomScale = getZoomScale();
		const float panx = (totalViewport_.width/2.0)/(zoomScale*stretch.x);
		const float pany = (totalViewport_.height/2.0)/(zoomScale*stretch.y);

		switch(viewType_){
		case XY_VIEW:
			return Vector2f(panx-mydataCoord.x/factor,
							pany-mydataCoord.y/factor);
		case XZ_VIEW:
			return Vector2f(panx-mydataCoord.x/factor,
							pany-mydataCoord.z/factor);
		case YZ_VIEW:
			return Vector2f(panx-mydataCoord.z/factor,
							pany-mydataCoord.y/factor);
		}

		throw OmArgException("invalid parameter");
	}

	inline void SetViewSliceOnPan()
	{
		const Vector2f translateVector = ComputePanDistance();

		const float pl = om::pow2int(getMipLevel());
		const float zoomFactor = getZoomScale();

		const DataCoord minDCoord(totalViewport_.lowerLeftX - translateVector.x * pl,
								  totalViewport_.lowerLeftY - translateVector.y * pl,
								  0);

		const DataCoord maxDCoord(totalViewport_.width / zoomFactor * pl - translateVector.x * pl,
								  totalViewport_.height / zoomFactor * pl - translateVector.y * pl,
								  0);

		setSliceMinAndMax(minDCoord, maxDCoord);

		OmEvents::Redraw3d();
	}

	inline void ChangeViewCenter()
	{
		const DataCoord depth = getViewSliceDepthData();

		const ScreenCoord crossCoord = DataToScreenCoord(depth);
		const ScreenCoord centerCoord = Vector2i(totalViewport_.width/2,
												 totalViewport_.height/2);

		const Vector2f currentPan = ComputePanDistance();
		const Vector2f newPan =  currentPan
			+ ScreenToPanShift(centerCoord - crossCoord);

		vgs_->SetPanDistance(viewType_, newPan);
		SetViewSliceOnPan();
	}

	inline void ResetWindowState()
	{
		static const NormCoord midPoint(0.5, 0.5, 0.5);
		const DataCoord depth = vol_->Coords().NormToDataCoord(midPoint);

		setSliceDepth(depth);

		vgs_->SetPanDistance(YZ_VIEW, Vector2f(0,0));
		vgs_->SetPanDistance(XZ_VIEW, Vector2f(0,0));
		vgs_->SetPanDistance(XY_VIEW, Vector2f(0,0));

		ZoomLevel()->Reset(getMaxMipLevel());

		OmTileCache::Clear();
		OmEvents::Redraw2d();
	}

	inline void SetPanDistance(const int x, const int y)
	{
		vgs_->SetPanDistance(viewType_, Vector2f(x, y));
		SetViewSliceOnPan();
	}

	inline void MoveUpStackCloserToViewer()
	{
		const int depth = GetDepthToDataSlice();
		const int numberOfSlicestoAdvance = om::pow2int(getMipLevel());
		float res = GetResOfDataSlice();
		SetDataSliceToDepth(depth+numberOfSlicestoAdvance*res);
	}

	inline void MoveDownStackFartherFromViewer()
	{
		const int depth = GetDepthToDataSlice();
		const int numberOfSlicestoAdvance = om::pow2int(getMipLevel());
		float res = GetResOfDataSlice();
		SetDataSliceToDepth(depth-numberOfSlicestoAdvance*res);
	}

    // mouse movement
	inline void DoMousePan(const Vector2i& cursorLocation)
	{
		const Vector2f current_pan = ComputePanDistance();
		const Vector2i drag = GetClickPoint() - cursorLocation;

		const DataCoord dragCoord = ScreenToDataCoord(drag);
		const DataCoord zeroCoord = ScreenToDataCoord(Vector2i(0,0));

		const DataCoord difference = zeroCoord - dragCoord;
		const DataCoord oldDepth = vgs_->GetViewDepthCoord();
		const DataCoord depth = oldDepth - difference;
		setSliceDepth(depth);
		OmEvents::ViewCenterChanged();

		SetClickPoint(cursorLocation.x, cursorLocation.y);
	}

	inline void DoMouseZoom(const int numSteps)
	{
		ZoomLevel()->MouseWheelZoom(numSteps, isLevelLocked_, getMaxMipLevel());

		// TODO: can the events be reordered/removed?
		OmEvents::ViewPosChanged();
		SetViewSliceOnPan();
		OmEvents::ViewCenterChanged();
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
		return ZoomLevel()->GetZoomScale();
	}
	inline int getMipLevel() const {
		return ZoomLevel()->GetMipLevel();
	}
	inline int getMaxMipLevel() const {
		return vol_->Coords().GetRootMipLevel();
	}
	inline OmZoomLevel* ZoomLevel() const {
		return vgs_->ZoomLevel();
	}

	// slice depth
	inline DataCoord getViewSliceDepthData() const
	{
		return DataCoord(getXsliceDepth(),
						  getYsliceDepth(),
						  getZsliceDepth());
	}
	inline int getSliceDepth() const {
		int depth = vgs_->GetViewSliceDepth(viewType_);
		return depth;
	}
	inline int getXsliceDepth() const {
		return vgs_->GetViewSliceDepth(YZ_VIEW);
	}
	inline int getYsliceDepth() const {
		return vgs_->GetViewSliceDepth(XZ_VIEW);
	}
	inline int getZsliceDepth() const {
		return vgs_->GetViewSliceDepth(XY_VIEW);
	}

	void setSliceDepth(const float d) const {
		vgs_->SetViewSliceDepth(viewType_, d);
	}
	void setXsliceDepth(const float d) const {
		vgs_->SetViewSliceDepth(YZ_VIEW, d);
	}
	void setYsliceDepth(const float d) const {
		vgs_->SetViewSliceDepth(XZ_VIEW, d);
	}
	void setZsliceDepth(const float d) const {
		vgs_->SetViewSliceDepth(XY_VIEW, d);
	}
	void setSliceDepth(const DataCoord& newDepth) const
	{
		setXsliceDepth(newDepth.x);
		setYsliceDepth(newDepth.y);
		setZsliceDepth(newDepth.z);
	}

	void setSliceMinAndMax(const DataCoord& min, const DataCoord& max)
	{
		if(viewType_ == YZ_VIEW) {
			vgs_->SetViewSliceMax(viewType_, max.y, max.x);
			vgs_->SetViewSliceMin(viewType_, min.y, min.x);
		} else {
			vgs_->SetViewSliceMax(viewType_, max.x, max.y);
			vgs_->SetViewSliceMin(viewType_, min.x, min.y);
		}
	}

	//refresh
	static void touchFreshnessAndRedraw2d()
	{
		OmCacheManager::TouchFresheness();
		OmEvents::Redraw2d();
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
									const Vector3<T>& scale) const {
		return OmView2dConverters::ScaleViewType(x, y, scale, viewType_);
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
		return vgs_->getBrushSize();
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

	// click point -- how is this different from mouse point???
	inline const ScreenCoord& GetClickPoint() const {
		return clickPoint_;
	}
	void SetClickPoint(const float x, const float y){
		clickPoint_.x = x;
		clickPoint_.y = y;
	}

	// camera moving
	inline bool IsCameraMoving() const {
		return cameraMoving_;
	}
	void SetCameraMoving(const bool isMoving){
		cameraMoving_ = isMoving;
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
	OmID GetSegmentationID() const {
		return 1;
	}

private:
	OmMipVolume* vol_;
	const ObjectType objType_;
	OmViewGroupState *const vgs_;
	const ViewType viewType_;
	const std::string name_;

	Vector4i totalViewport_; //lower left x, lower left y, width, height
	bool scribbling_;
	Vector2i mousePoint_;
	bool isLevelLocked_;

	ScreenCoord clickPoint_;
	bool cameraMoving_;

	// (x,y) coordinates only (no depth); needed for Bresenham
	DataCoord lastDataPoint_;
};

#endif
