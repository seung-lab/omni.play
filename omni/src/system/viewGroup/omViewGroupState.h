#ifndef OM_VIEW_GROUP_STATE_H
#define OM_VIEW_GROUP_STATE_H

#include "system/omGenericManager.h"
#include "system/omManageableObject.h"
#include "system/omStateManager.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentColorizer.h"
#include <boost/tr1/unordered_map.hpp>

class OmViewGroupState : public OmManageableObject {
 public:
	OmViewGroupState();

	void SetSegmentation( const OmId  );
	void SetChannel( const OmId  );

	//viewbox state
	void SetViewSliceMin(ViewType, Vector2<float>, bool postEvent = true);
	Vector2<float> GetViewSliceMin(ViewType);
	
	void SetViewSliceMax(ViewType, Vector2<float>, bool postEvent = true);
	Vector2<float> GetViewSliceMax(ViewType);

	SpaceCoord GetViewDepthCoord();	
	void SetViewSliceDepth(ViewType, float, bool postEvent = true);
	float GetViewSliceDepth(ViewType);
	
	void SetZoomLevel(Vector2<int>);
	Vector2<int> GetZoomLevel();
	
	void SetPanDistance(ViewType, Vector2f, bool postEvent = true);
	Vector2f GetPanDistance(ViewType);
		
	// slices
	void SetSliceState(OmSlicePlane plane, bool enabled);

	void SetViewSliceDataFormat(int bytesPerSample);
	void SetViewSlice(const OmSlicePlane plane, const Vector3<int> &dim, unsigned char *data);

	void ColorTile( SEGMENT_DATA_TYPE *, const int,
			const ObjectType, unsigned char * );

	void setBreakThreshold(int t){ mBreakThreshold = t; }
	int getBreakThreshold(){ return mBreakThreshold; }

 private:
	int mBreakThreshold;

	//view event
	float mXYSlice[6], mYZSlice[6], mXZSlice[6];
	float mXYPan[2], mYZPan[2], mXZPan[2];
	Vector2<int> zoom_level;
	
	SpaceBbox mViewBbox;
	SpaceCoord mViewCenter;
	
	bool mXYSliceEnabled, mYZSliceEnabled, mXZSliceEnabled;
	
	int mViewSliceBytesPerSample, mViewSliceSamplesPerPixel;
	Vector3i mViewSliceDimXY, mViewSliceDimYZ, mViewSliceDimXZ;
	unsigned char *mViewSliceDataXY, *mViewSliceDataYZ, *mViewSliceDataXZ;

	boost::unordered_map< int, OmSegmentColorizer * > mColorCaches;

	SegmentationDataWrapper m_sdw;
	ChannelDataWrapper m_cdw;

};

#endif
