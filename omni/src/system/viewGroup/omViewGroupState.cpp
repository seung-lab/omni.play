#include "common/omDebug.h"
#include "system/viewGroup/omViewGroupState.h"
#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"

OmViewGroupState::OmViewGroupState()
{
	mViewSliceDataXY = NULL;
	mViewSliceDataYZ = NULL;
	mViewSliceDataXZ = NULL;

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
}


/////////////////////////////////
///////          View

/*
 *	Set/Get minimum coordiante of view slice.
 */
void OmViewGroupState::SetViewSliceMin(ViewType plane, Vector2 < float >point, bool postEvent)
{
	switch (plane) {
	case XY_VIEW:
		mXYSlice[0] = point.x;
		mXYSlice[1] = point.y;
		break;
	case XZ_VIEW:
		mXZSlice[0] = point.x;
		mXZSlice[1] = point.y;
		break;
	case YZ_VIEW:
		mYZSlice[0] = point.x;
		mYZSlice[1] = point.y;
		break;
	default:
		assert(false);
	}

	if (postEvent) {
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
	}
}

Vector2 < float > OmViewGroupState::GetViewSliceMin(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2 < float >(&mXYSlice[0]);
	case XZ_VIEW:
		return Vector2 < float >(&mXZSlice[0]);
	case YZ_VIEW:
		return Vector2 < float >(&mYZSlice[0]);
	default:
		assert(false);
	}
}

/*
 *	Set/Get maximum coordiante of view slice.
 */
void OmViewGroupState::SetViewSliceMax(ViewType plane, Vector2 < float >point, bool postEvent)
{
	switch (plane) {
	case XY_VIEW:
		mXYSlice[2] = point.x;
		mXYSlice[3] = point.y;
		break;
	case XZ_VIEW:
		mXZSlice[2] = point.x;
		mXZSlice[3] = point.y;
		break;
	case YZ_VIEW:
		mYZSlice[2] = point.x;
		mYZSlice[3] = point.y;
		break;
	default:
		assert(false);
	}

	if (postEvent) {
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
	}
}

Vector2 < float > OmViewGroupState::GetViewSliceMax(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2f(&mXYSlice[2]);
	case XZ_VIEW:
		return Vector2f(&mXZSlice[2]);
	case YZ_VIEW:
		return Vector2f(&mYZSlice[2]);
	default:
		assert(false);
	}
}

/**
 *	Set/Get depth of view slice.
 */
void OmViewGroupState::SetViewSliceDepth(ViewType plane, float depth, bool postEvent)
{
	if (ISNAN(depth)) assert(0);
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
		assert(false);
	}

	if (postEvent) {
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
	}
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
		assert(false);
	}
}

/*
 *	Set/Get zoom level.
 */
void OmViewGroupState::SetZoomLevel(Vector2 < int >zoom)
{
	zoom_level = zoom;

	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_POS_CHANGE));
}

Vector2 < int > OmViewGroupState::GetZoomLevel()
{
	return zoom_level;
}

/*
 *	Set/Get pan distance.
 */
void OmViewGroupState::SetPanDistance(ViewType plane, Vector2f pan, bool postEvent)
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
		assert(false);
	}

	if (postEvent) {
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_POS_CHANGE));
	}
}

Vector2f OmViewGroupState::GetPanDistance(ViewType plane)
{
	switch (plane) {
	case XY_VIEW:
		return Vector2f(mXYPan[0], mXYPan[1]);
	case XZ_VIEW:
		return Vector2f(mXZPan[0], mXZPan[1]);
	case YZ_VIEW:
		return Vector2f(mYZPan[0], mYZPan[1]);
	default:
		assert(false);
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

/*
 *	Sets the data format for the slice image data.  This will automatically clear
 *	any image data previously set.
 */
void OmViewGroupState::SetViewSliceDataFormat(int bytesPerSample)
{
	SetViewSlice(SLICE_XY_PLANE, Vector3 < int >::ZERO, NULL);
	SetViewSlice(SLICE_YZ_PLANE, Vector3 < int >::ZERO, NULL);
	SetViewSlice(SLICE_XZ_PLANE, Vector3 < int >::ZERO, NULL);

	mViewSliceBytesPerSample = bytesPerSample;
}

/*
 *	Sets slice dimensions and copies image data (deletes old image data if necessary).  
 *	note: Uses previously specified format to perform deep copy of given data.
 */
void OmViewGroupState::SetViewSlice(const OmSlicePlane plane, const Vector3 < int >&dim, unsigned char *p_data)
{
	//get size of image data
	unsigned int data_size = mViewSliceBytesPerSample * dim.x * dim.y * dim.z;

	//alloc and copy
	unsigned char *p_data_copy = new unsigned char[data_size];
	memcpy(p_data_copy, p_data, data_size);

	switch (plane) {

	case SLICE_XY_PLANE:
		mViewSliceDimXY = dim;
		if (mViewSliceDataXY) {
			delete mViewSliceDataXY;
		}
		mViewSliceDataXY = p_data_copy;
		break;

	case SLICE_YZ_PLANE:
		mViewSliceDimYZ = dim;
		if (mViewSliceDataYZ) {
			delete mViewSliceDataYZ;
		}
		mViewSliceDataYZ = p_data_copy;
		break;

	case SLICE_XZ_PLANE:
		mViewSliceDimXZ = dim;
		if (mViewSliceDataXZ) {
			delete mViewSliceDataXZ;
		}
		mViewSliceDataXZ = p_data_copy;
		break;

	default:
		assert(false);
	}
}

void OmViewGroupState::SetSegmentation( const OmId segID ) 
{ 
	m_sdw = SegmentationDataWrapper( segID ); 
}

void OmViewGroupState::SetChannel( const OmId chanID )
{ 
	m_cdw = ChannelDataWrapper( chanID );
}

extern bool mShatter;
void OmViewGroupState::ColorTile( SEGMENT_DATA_TYPE * imageData, const int size,
				  const ObjectType objType, unsigned char * data )
{
	OmSegmentColorCacheType sccTypeEnum;

	switch( objType ){
	case CHANNEL:
		if( mShatter ){
			sccTypeEnum = ChannelBreak;
		} else {
			sccTypeEnum = Channel;
		}
		break;

	case SEGMENTATION:
		if( mShatter ) {
			sccTypeEnum = SegmentationBreak;
		} else {
			sccTypeEnum = Segmentation;
		}
		break;
	default:
		assert(0);
	}

	int sccTypeInt = (int)sccTypeEnum;

	if( 0 == mColorCaches.count( sccTypeInt ) ){
		mColorCaches[ sccTypeInt ] = new OmSegmentColorizer( m_sdw.getSegmentCache(), sccTypeEnum);
	}
	
	mColorCaches[ sccTypeInt ]->colorTile( imageData, size, data, this );
}

