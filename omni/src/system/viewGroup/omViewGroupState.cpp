#include "system/viewGroup/omBrushSize.hpp"
#include "common/omDebug.h"
#include "gui/toolbars/toolbarManager.h"
#include "gui/viewGroup.h"
#include "segment/omSegment.h"
#include "segment/omSegmentColorizer.h"
#include "system/cache/omCacheManager.h"
#include "system/omEvents.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "tiles/omTileCoord.h"

#include <boost/make_shared.hpp>

OmViewGroupState::OmViewGroupState( MainWindow * mw)
	: OmManageableObject()
	, mMainWindow(mw)
	, mViewGroup(boost::make_shared<ViewGroup>(mMainWindow, this))
	, brushSize_(boost::make_shared<OmBrushSize>())
	, segmentBeingSplit_(boost::make_shared<SegmentDataWrapper>())
{
	mXYSliceEnabled = false;
	mYZSliceEnabled = false;
	mXZSliceEnabled = false;

	mYZSlice[4] = 0.0;
	mXZSlice[4] = 0.0;
	mXYSlice[4] = 0.0;

	mXYPan[0] = 0.0;
	mXYPan[1] = 0.0;
	mYZPan[0] = 0.0;
	mYZPan[1] = 0.0;
	mXZPan[0] = 0.0;
	mXZPan[1] = 0.0;

	mBreakThreshold = 0;
	mDustThreshold = 90;
	mShatter = false;
	mSplitting = false;
	mBreakOnSplit = true;
	mShowValid = false;
	mShowSplit = false;
	mShowValidInColor = false;
	mShowFilterInColor = false;

	zoom_level = Vector2i(0, 6);

	mColorCaches.resize(SCC_NUMBER_OF_ENUMS);

	//debug(viewgroupstate, "constructed viewGroupState\n");
}

// GUI state
void OmViewGroupState::addView2Dchannel(OmId chan_id, ViewType vtype)
{
	mViewGroup->AddView2Dchannel(chan_id, vtype);
}

void OmViewGroupState::addView2Dsegmentation(OmId segmentation_id,
											 ViewType vtype)
{
	mViewGroup->AddView2Dsegmentation(segmentation_id, vtype);
}

void OmViewGroupState::addView3D()
{
	mViewGroup->AddView3D();
}

void OmViewGroupState::addAllViews( OmId channelID, OmId segmentationID )
{
	mViewGroup->AddAllViews( channelID, segmentationID );
}

/////////////////////////////////
///////          View

/*
 *	Set/Get minimum coordiante of view slice.
 */
void OmViewGroupState::SetViewSliceMin(const ViewType plane, const float x,
									   const float y)
{
	switch (plane) {
	case XY_VIEW:
		mXYSlice[0] = x;
		mXYSlice[1] = y;
		break;
	case XZ_VIEW:
		mXZSlice[0] = x;
		mXZSlice[1] = y;
		break;
	case YZ_VIEW:
		mYZSlice[0] = x;
		mYZSlice[1] = y;
		break;
	default:
		assert(false);
	}
}

Vector2f OmViewGroupState::GetViewSliceMin(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2f(&mXYSlice[0]);
	case XZ_VIEW:
		return Vector2f(&mXZSlice[0]);
	case YZ_VIEW:
		return Vector2f(&mYZSlice[0]);
	default:
		assert(false);
	}
}

/*
 *	Set/Get maximum coordiante of view slice.
 */
void OmViewGroupState::SetViewSliceMax(const ViewType plane, const float x,
									   const float y)
{
	switch (plane) {
	case XY_VIEW:
		mXYSlice[2] = x;
		mXYSlice[3] = y;
		break;
	case XZ_VIEW:
		mXZSlice[2] = x;
		mXZSlice[3] = y;
		break;
	case YZ_VIEW:
		mYZSlice[2] = x;
		mYZSlice[3] = y;
		break;
	default:
		throw OmArgException("unknown plane");
	}
}

Vector2f OmViewGroupState::GetViewSliceMax(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2f(&mXYSlice[2]);
	case XZ_VIEW:
		return Vector2f(&mXZSlice[2]);
	case YZ_VIEW:
		return Vector2f(&mYZSlice[2]);
	default:
		throw OmArgException("unknown plane");
	}
}

/**
 *	Set/Get depth of view slice.
 */
void OmViewGroupState::SetViewSliceDepth(ViewType plane, float depth)
{
	if(std::isnan(depth)){
		throw OmArgException("depth was not a number");
	}

	switch (plane) {
	case XY_VIEW:
		mXYSlice[4] = depth;
		break;
	case XZ_VIEW:
		mXZSlice[4] = depth;
		break;
	case YZ_VIEW:
		mYZSlice[4] = depth;
		break;
	default:
		throw OmArgException("unknown plane");
	}

	OmEvents::ViewBoxChanged();
}

SpaceCoord OmViewGroupState::GetViewDepthCoord()
{
	SpaceCoord spacec;
	spacec.x = mYZSlice[4];
	spacec.y = mXZSlice[4];
	spacec.z = mXYSlice[4];
	return spacec;
}

float OmViewGroupState::GetViewSliceDepth(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return mXYSlice[4];
	case XZ_VIEW:
		return mXZSlice[4];
	case YZ_VIEW:
		return mYZSlice[4];
	default:
		throw OmArgException("unknown plane");
	}
}

/*
 *	Set/Get zoom level.
 */
void OmViewGroupState::SetZoomLevel(const Vector2i& zoom)
{
	zoom_level = zoom;
	OmEvents::ViewPosChanged();
}

/*
 *	Set/Get pan distance.
 */
void OmViewGroupState::SetPanDistance(const ViewType plane,
									  const Vector2f& pan)
{
	switch (plane) {
	case XY_VIEW:
		mXYPan[0] = pan.x;
		mXYPan[1] = pan.y;
		break;
	case XZ_VIEW:
		mXZPan[0] = pan.x;
		mXZPan[1] = pan.y;
		break;
	case YZ_VIEW:
		mYZPan[0] = pan.x;
		mYZPan[1] = pan.y;
		break;
	default:
		throw OmArgException("unknown plane");
	}

	OmEvents::ViewPosChanged();
}

Vector2f OmViewGroupState::ComputePanDistance(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2f(mXYPan[0], mXYPan[1]);
	case XZ_VIEW:
		return Vector2f(mXZPan[0], mXZPan[1]);
	case YZ_VIEW:
		return Vector2f(mYZPan[0], mYZPan[1]);
	default:
		throw OmArgException("unknown plane");
	}
}

/*
 *	Enable/disable orthogonal slice.
 */
void OmViewGroupState::SetSliceState(OmSlicePlane plane, bool enabled)
{
	switch (plane) {

	case SLICE_XY_PLANE:
		mXYSliceEnabled = enabled;
		break;

	case SLICE_YZ_PLANE:
		mYZSliceEnabled = enabled;
		break;

	case SLICE_XZ_PLANE:
		mXZSliceEnabled = enabled;
		break;

	default:
		assert(false);
	}
}

OmSegmentColorCacheType
OmViewGroupState::determineColorizationType(const ObjectType objType)
{
	switch( objType ){
	case CHANNEL:
		if(mShowValid) {
			if(mShowValidInColor){
				return SCC_FILTER_VALID;
			}
			return SCC_FILTER_VALID_BLACK;
		}

		if(shouldVolumeBeShownBroken()){
			return SCC_FILTER_BREAK;
		}

		if(ShowNonSelectedSegmentsInColorInFilter()){
			return SCC_FILTER_COLOR;
		}

		return SCC_FILTER_BLACK;

	case SEGMENTATION:
		if(mShowValid){
			if(mShowValidInColor){
				return SCC_SEGMENTATION_VALID;
			}
			return SCC_SEGMENTATION_VALID_BLACK;
		}

		if(shouldVolumeBeShownBroken() ){
			return SCC_SEGMENTATION_BREAK;
		}

		return SCC_SEGMENTATION;
	}

	throw OmArgException("unknown objType");
}

void OmViewGroupState::setupColorizer(const Vector2i& dims,
									  const OmTileCoord& key,
									  const OmSegmentColorCacheType sccType)
{
	zi::guard g(mColorCacheMapLock);

	if(NULL == mColorCaches[sccType]){
		SegmentationDataWrapper sdw(key.getVolume()->getID());

		OmSegmentColorizer* sc =
			new OmSegmentColorizer(sdw.getSegmentCache(),
								   sccType,
								   dims);
		mColorCaches[ sccType ] =
			boost::shared_ptr<OmSegmentColorizer>(sc);
	}
}

boost::shared_ptr<OmColorRGBA>
OmViewGroupState::ColorTile(boost::shared_ptr<uint32_t> imageData,
							const Vector2i& dims,
							const OmTileCoord& key)
{
	if(SEGMENTATION != key.getVolume()->getVolumeType()){
		throw OmIoException("can only color segmentations");
	}

	const OmSegmentColorCacheType sccType =
		key.getSegmentColorCacheType();

	setupColorizer(dims, key, sccType);
	return mColorCaches[ sccType ]->colorTile(imageData);
}

void OmViewGroupState::SetToolBarManager(ToolBarManager * tbm)
{
	mToolBarManager = tbm;
}

void OmViewGroupState::SetShatterMode(bool shatter)
{
	mShatter = shatter;
}

bool OmViewGroupState::GetShatterMode()
{
	return mShatter;
}

void OmViewGroupState::ToggleShatterMode()
{
	mShatter = !mShatter;
}

std::pair<bool, SegmentDataWrapper> OmViewGroupState::GetSplitMode()
{
	return std::make_pair(mSplitting, *segmentBeingSplit_);
}

void OmViewGroupState::SetSplitMode(bool onoroff, bool postEvent)
{
	mSplitting = onoroff;
	if(false == onoroff){
		if(postEvent) {
			mToolBarManager->SetSplittingOff();
		}
		SetShowSplitMode(false);
		OmStateManager::SetOldToolModeAndSendEvent();
	}

	OmCacheManager::TouchFresheness();
	OmEvents::Redraw3d();
	OmEvents::Redraw();
}

void OmViewGroupState::SetSplitMode(const SegmentDataWrapper& sdw)
{
	segmentBeingSplit_->set(sdw);
	SetSplitMode(true);
}

void OmViewGroupState::SetBreakOnSplitMode(bool mode)
{
	mBreakOnSplit = mode;
}

void OmViewGroupState::SetShowValidMode(bool mode, bool inColor)
{
	//debug(valid, "OmViewGroupState::SetShowValidMode(bool mode=%i, bool inColor=%i)\n", mode, inColor);
	OmCacheManager::TouchFresheness();
	mShowValid = mode;
	mShowValidInColor = inColor;
	OmEvents::Redraw3d();
	OmEvents::Redraw();
}

bool OmViewGroupState::ShowNonSelectedSegmentsInColorInFilter()
{
	return mShowFilterInColor;
}

void OmViewGroupState::SetHowNonSelectedSegmentsAreColoredInFilter(const bool inColor)
{
	mShowFilterInColor = inColor;
	OmCacheManager::TouchFresheness();
	OmEvents::Redraw();
}

void OmViewGroupState::SetShowSplitMode(bool mode)
{
	OmCacheManager::TouchFresheness();
	OmEvents::Redraw3d();
	OmEvents::Redraw();
	mShowSplit = mode;
}

bool OmViewGroupState::shouldVolumeBeShownBroken()
{
	return mShatter || (mShowSplit && mBreakOnSplit);
}

void OmViewGroupState::setTool(const OmToolMode tool)
{
	mToolBarManager->setTool(tool);
}

