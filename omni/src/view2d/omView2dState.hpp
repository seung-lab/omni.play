#ifndef OM_VIEW2D_STATE_HPP
#define OM_VIEW2D_STATE_HPP

#include "common/omDebug.h"
#include "system/cache/omCacheManager.h"
#include "system/omEvents.h"
#include "system/omLocalPreferences.h"
#include "system/omProjectData.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"
#include "viewGroup/omZoomLevel.hpp"
#include <QSize>

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
		, mViewGroupState(vgs)
		, mViewType(viewType)
		, name_(name)
		, mScribbling(false)
		, mousePoint_(0,0)
		, mLevelLock(false)
		, clickPoint_(0,0)
		, cameraMoving_(false)
		, lastDataPoint_(0,0)
		, zoomLevel_(vgs->ZoomLevel())
	{
		setTotalViewport(size);
		zoomLevel_->Update(vol_);
	}

	// coord convertors
	Vector2f ScreenToPanShift(const Vector2i& screenshift) const
	{
		const Vector2f stretch= vol_->GetStretchValues(mViewType);
		const float zoomScale = getZoomScale();
		const float panx = screenshift.x/zoomScale/stretch.x;
		const float pany = screenshift.y/zoomScale/stretch.y;
		return Vector2f(panx, pany);
	}

	SpaceCoord ScreenToSpaceCoord(const ScreenCoord& screenc) const
	{
		const Vector2f mPanDistance = ComputePanDistance();
		const Vector2f stretch = vol_->GetStretchValues(mViewType);

		const Vector3f dataScale = vol_->GetDataDimensions();
		const float factor = om::pow2int(getMipLevel());
		const float zoomScale = getZoomScale();

		const float unscaleNormX = ((float)screenc.x/zoomScale/stretch.x-mPanDistance.x)*factor;
		const float unscaleNormY = ((float)screenc.y/zoomScale/stretch.y-mPanDistance.y)*factor;

		const NormCoord normc = scaleViewType(unscaleNormX, unscaleNormY,
											  dataScale);

		SpaceCoord result = vol_->NormToSpaceCoord(normc);
		setViewTypeDepth(result, getSliceDepth());
		return result;
	}

	DataCoord ScreenToDataCoord(const ScreenCoord & screenc) const {
		return vol_->SpaceToDataCoord(ScreenToSpaceCoord(screenc));
	}

	ScreenCoord SpaceToScreenCoord(const SpaceCoord& spacec) const
	{
		const NormCoord normCoord = vol_->SpaceToNormCoord(spacec);
		const Vector3f scale = vol_->GetDataDimensions();
		const Vector3f datac = normCoord * scale;

		const Vector2f mPanDistance = ComputePanDistance();
		const Vector2f stretch = vol_->GetStretchValues(mViewType);
		const float factor = om::pow2int(getMipLevel());
		const float zoomScale = getZoomScale();

		switch(mViewType){
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

	int GetDepthToDataSlice() const
	{
		const SpaceCoord depthCoord = mViewGroupState->GetViewDepthCoord();
		const DataCoord dataCoord = vol_->SpaceToDataCoord(depthCoord);
		return getViewTypeDepth(dataCoord);
	}

	void SetDataSliceToDepth(const int slice) const
	{
		DataCoord dataCoord(0, 0, 0);
		setViewTypeDepth(dataCoord, slice);
		const SpaceCoord depthCoord = vol_->DataToSpaceCoord(dataCoord);

		setSliceDepth(getViewTypeDepth(depthCoord));
	}

	Vector2f ComputePanDistance() const
	{
		const Vector2f stretch = vol_->GetStretchValues(mViewType);
		const float factor = om::pow2int(getMipLevel());

		const DataCoord mydataCoord = getViewSliceDepthData();

		const float zoomScale = getZoomScale();
		const float panx = (mTotalViewport.width/2.0)/(zoomScale*stretch.x);
		const float pany = (mTotalViewport.height/2.0)/(zoomScale*stretch.y);

		switch(mViewType){
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

	void SetViewSliceOnPan()
	{
		const Vector2f translateVector = ComputePanDistance();

		const float pl = om::pow2int(getMipLevel());
		const float zoomFactor = getZoomScale();

		const DataCoord minDCoord(mTotalViewport.lowerLeftX - translateVector.x * pl,
								  mTotalViewport.lowerLeftY - translateVector.y * pl,
								  0);

		const DataCoord maxDCoord(mTotalViewport.width / zoomFactor * pl - translateVector.x * pl,
								  mTotalViewport.height / zoomFactor * pl - translateVector.y * pl,
								  0);

		setSliceMinAndMax(vol_->DataToSpaceCoord(minDCoord),
						  vol_->DataToSpaceCoord(maxDCoord));

		OmEvents::Redraw3d();
	}

	void PanAndZoom(const int mipLevel, const int zoomFactor)
	{
		zoomLevel_->SetZoomLevel(mipLevel, zoomFactor);
		OmEvents::ViewPosChanged();

		SetViewSliceOnPan();
		OmEvents::ViewCenterChanged();
	}

	void ChangeViewCenter()
	{
		const SpaceCoord depth = getViewSliceDepthSpace();

		const ScreenCoord crossCoord = SpaceToScreenCoord(depth);
		const ScreenCoord centerCoord = Vector2i(mTotalViewport.width/2,
												 mTotalViewport.height/2);

		const Vector2f currentPan = ComputePanDistance();
		const Vector2f newPan =  currentPan
			+ ScreenToPanShift(centerCoord - crossCoord);

		mViewGroupState->SetPanDistance(mViewType, newPan);
		SetViewSliceOnPan();
	}

	void ResetWindowState()
	{
		const SpaceCoord depth =
			vol_->NormToSpaceCoord(NormCoord(0.5, 0.5, 0.5));

		setSliceDepth(depth);

		mViewGroupState->SetPanDistance(YZ_VIEW, Vector2f(0,0));
		mViewGroupState->SetPanDistance(XZ_VIEW, Vector2f(0,0));
		mViewGroupState->SetPanDistance(XY_VIEW, Vector2f(0,0));

		//TODO: clear cache?
	}

	void SetPanDistance(const int x, const int y)
	{
		mViewGroupState->SetPanDistance(mViewType,
										Vector2f(x, y));
		SetViewSliceOnPan();
	}

	void MoveUpStackCloserToViewer()
	{
		const int depth = GetDepthToDataSlice();
		const int numberOfSlicestoAdvance = om::pow2int(getMipLevel());
		SetDataSliceToDepth(depth+numberOfSlicestoAdvance);
	}

	void MoveDownStackFartherFromViewer()
	{
		const int depth = GetDepthToDataSlice();
		const int numberOfSlicestoAdvance = om::pow2int(getMipLevel());
		SetDataSliceToDepth(depth-numberOfSlicestoAdvance);
	}

	// mouse movement
	void mouseMove_CamMoving(const Vector2i& cursorLocation)
	{
		const Vector2f current_pan = ComputePanDistance();
		const Vector2i drag = GetClickPoint() - cursorLocation;

		const SpaceCoord dragCoord = ScreenToSpaceCoord(drag);
		const SpaceCoord zeroCoord = ScreenToSpaceCoord(Vector2i(0,0));

		const SpaceCoord difference = zeroCoord - dragCoord;
		const SpaceCoord oldDepth = getViewGroupState()->GetViewDepthCoord();
		const SpaceCoord depth = oldDepth - difference;
		setSliceDepth(depth);
		OmEvents::ViewCenterChanged();

		SetClickPoint(cursorLocation.x, cursorLocation.y);
	}

	// name
	const std::string& getName() const {
		return name_;
	}

	// viewtype
	ViewType getViewType() const {
		return mViewType;
	}

	// viewport
	const Vector4i& getTotalViewport() const {
		return mTotalViewport;
	}
	void setTotalViewport(const QSize& size){
		mTotalViewport = Vector4i(0, 0, size.width(), size.height());
	}
	void setTotalViewport(const int width, const int height){
		mTotalViewport = Vector4i(0, 0, width, height);
	}

	// volume
	OmMipVolume* getVol() const {
		return vol_;
	}
	void setVol(OmMipVolume* vol){
		vol_ = vol;
	}

	// view group state
	OmViewGroupState* getViewGroupState() const {
		return mViewGroupState;
	}

	// scribbling
	bool getScribbling(){
		return mScribbling;
	}
	void setScribbling(const bool s){
		mScribbling = s;
	}

	// tile cache
	const boost::shared_ptr<OmTileCache>& getCache() const {
		return OmProjectData::getTileCache();
	}

	// zoom and mip level
	float getZoomScale() const {
		return zoomLevel_->GetZoomScale();
	}
	int getMipLevel() const {
		return zoomLevel_->GetMipLevel();
	}

	// slice depth
	SpaceCoord getViewSliceDepthSpace() const
	{
		return SpaceCoord(getXsliceDepth(),
						  getYsliceDepth(),
						  getZsliceDepth());
	}
	DataCoord getViewSliceDepthData() const {
		return vol_->SpaceToDataCoord(getViewSliceDepthSpace());
	}
	float getSliceDepth() const {
		return mViewGroupState->GetViewSliceDepth(mViewType);
	}
	float getXsliceDepth() const {
		return mViewGroupState->GetViewSliceDepth(YZ_VIEW);
	}
	float getYsliceDepth() const {
		return mViewGroupState->GetViewSliceDepth(XZ_VIEW);
	}
	float getZsliceDepth() const {
		return mViewGroupState->GetViewSliceDepth(XY_VIEW);
	}

	void setSliceDepth(const float d) const {
		mViewGroupState->SetViewSliceDepth(mViewType, d);
	}
	void setXsliceDepth(const float d) const {
		mViewGroupState->SetViewSliceDepth(YZ_VIEW, d);
	}
	void setYsliceDepth(const float d) const {
		mViewGroupState->SetViewSliceDepth(XZ_VIEW, d);
	}
	void setZsliceDepth(const float d) const {
		mViewGroupState->SetViewSliceDepth(XY_VIEW, d);
	}
	void setSliceDepth(const SpaceCoord& newDepth) const
	{
		setXsliceDepth(newDepth.x);
		setYsliceDepth(newDepth.y);
		setZsliceDepth(newDepth.z);
	}

	void setSliceMinAndMax(const SpaceCoord& min, const SpaceCoord& max)
	{
		if(mViewType == YZ_VIEW) {
			mViewGroupState->SetViewSliceMax(mViewType, max.y, max.x);
			mViewGroupState->SetViewSliceMin(mViewType, min.y, min.x);
		} else {
			mViewGroupState->SetViewSliceMax(mViewType, max.x, max.y);
			mViewGroupState->SetViewSliceMin(mViewType, min.x, min.y);
		}
	}

	//refresh
	static void touchFreshnessAndRedraw()
	{
		OmCacheManager::TouchFresheness();
		OmEvents::Redraw();
	}

	//helpers
	template <typename T>
	Vector3<T> makeViewTypeVector3(const Vector3<T>& vec) const {
		return makeViewTypeVector3(vec.x, vec.y, vec.z);
	}

	template <typename T>
	Vector3<T> makeViewTypeVector3(const T& x, const T& y,
								   const T& z) const
	{
		switch(mViewType){
		case XY_VIEW:
			return Vector3<T>(x, y, z);
		case XZ_VIEW:
			return Vector3<T>(x, z, y);
		case YZ_VIEW:
			return Vector3<T>(z, y, x);
		default:
			throw OmArgException("invalid viewType");
		}
	}

	template <typename T>
	T getViewTypeDepth(const Vector3<T>& vec) const
	{
		switch(mViewType){
		case XY_VIEW:
			return vec.z;
		case XZ_VIEW:
			return vec.y;
		case YZ_VIEW:
			return vec.x;
		default:
			throw OmArgException("invalid viewType");
		}
	}

	template <typename T>
	void setViewTypeDepth(Vector3<T>& vec, const T& val) const
	{
		switch(mViewType){
		case XY_VIEW:
			vec.z = val;
			break;
		case XZ_VIEW:
			vec.y = val;
			break;
		case YZ_VIEW:
			vec.x = val;
			break;
		default:
			throw OmArgException("invalid viewType");
		}
	}

	template <typename T>
	Vector3<T> scaleViewType(const T& x, const T& y,
							 const Vector3<T>& scale) const
	{
		switch(mViewType){
		case XY_VIEW:
			return Vector3<T>(x / scale.x,
							  y / scale.y,
							  0);
		case XZ_VIEW:
			return Vector3<T>(x / scale.x,
							  0,
							  y / scale.z);
		case YZ_VIEW:
			return Vector3<T>(0,
							  y / scale.y,
							  x / scale.z);
		default:
			throw OmArgException("invalid viewType");
		}
	}

	// for tile coord creation
	SpaceCoord computeTileSpaceCoord(const DataCoord& dataCoord) const
	{
		const DataBbox box = vol_->DataToDataBBox(dataCoord, 0);
		const int depth = getViewTypeDepth(dataCoord);

		Vector3i bcoord = box.getMin();
		setViewTypeDepth(bcoord, depth);

		return vol_->DataToSpaceCoord(bcoord);
	}

	// brush size
	boost::shared_ptr<OmBrushSize>& getBrushSize(){
		return mViewGroupState->getBrushSize();
	}

	// mouse point
	const Vector2i& GetMousePoint() const {
		return mousePoint_;
	}
	void SetMousePoint(const int x, const int y){
		mousePoint_ = Vector2i(x,y);
	}

	// mip level lock
	bool IsLevelLocked(){
		return mLevelLock;
	}
	void ToggleLevelLock(){
		mLevelLock = !mLevelLock;
	}

	// click point -- how is this different from mouse point???
	const ScreenCoord& GetClickPoint(){
		return clickPoint_;
	}
	void SetClickPoint(const float x, const float y){
		clickPoint_.x = x;
		clickPoint_.y = y;
	}

	// camera moving
	bool IsCameraMoving(){
		return cameraMoving_;
	}
	void SetCameraMoving(const bool isMoving){
		cameraMoving_ = isMoving;
	}

	// mouse click coord
	DataCoord ComputeMouseClickPointDataCoord(QMouseEvent* event){
		return ScreenToDataCoord(Vector2f(event->x(), event->y()));
	}

	// last data point--coupled w/ click point?
	const DataCoord& GetLastDataPoint(){
		return lastDataPoint_;
	}
	void SetLastDataPoint(const DataCoord& coord){
		lastDataPoint_ = coord;
	}

	// whether overall view2d widget is displaying a channel or a segmentation
	ObjectType getObjectType() const {
		return objType_;
	}

private:
	OmMipVolume* vol_;
	const ObjectType objType_;
	OmViewGroupState *const mViewGroupState;
	const ViewType mViewType;
	const std::string name_;

	Vector4i mTotalViewport; //lower left x, lower left y, width, height
	bool mScribbling;
	Vector2i mousePoint_;
	bool mLevelLock;

	ScreenCoord clickPoint_;
	bool cameraMoving_;

	// FLAT data coordinates, not accurate for orthogonal views but accurate for Bresenham
	DataCoord lastDataPoint_;

	boost::shared_ptr<OmZoomLevel> zoomLevel_;
};

#endif
