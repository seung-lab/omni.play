#ifndef OM_VIEW_GROUP_STATE_H
#define OM_VIEW_GROUP_STATE_H

#include "system/omManageableObject.h"
#include "zi/omMutex.h"

class FilterWidget;
class InspectorProperties;
class MainWindow;
class OmBrushSize;
class OmMipVolume;
class OmSegment;
class OmSegmentColorizer;
class OmTileCoord;
class OmZoomLevel;
class SegmentDataWrapper;
class ToolBarManager;
class ViewGroup;

class OmViewGroupState : public OmManageableObject {
public:
	OmViewGroupState( MainWindow * mw );
	~OmViewGroupState(){}

	void SetInspectorProperties(InspectorProperties * ip) {mInspectorProperties = ip; }
	InspectorProperties * GetInspectorProperties() { return mInspectorProperties; }

	void SetFilterWidget(FilterWidget* f){ mFilterWidget = f; }
	FilterWidget* GetFilterWidget(){ return mFilterWidget; }

	// GUI state
	void addView2Dchannel( OmID chan_id, ViewType vtype);
	void addView2Dsegmentation( OmID segmentation_id, ViewType vtype);
	void addView3D();
	void addAllViews( OmID channelID, OmID segmentationID );

	//viewbox state
	void SetViewSliceMin(const ViewType plane, const float x,
						 const float y);
	Vector2f GetViewSliceMin(ViewType);

	void SetViewSliceMax(const ViewType plane, const float x,
						 const float y);
	Vector2f GetViewSliceMax(ViewType);

	SpaceCoord GetViewDepthCoord();
	void SetViewSliceDepth(ViewType, float);
	float GetViewSliceDepth(ViewType);

	boost::shared_ptr<OmZoomLevel>& ZoomLevel() {
		return zoomLevel_;
	}

	void SetPanDistance(const ViewType, const Vector2f&);
	Vector2f ComputePanDistance(ViewType);

	boost::shared_ptr<OmColorRGBA> ColorTile(uint32_t const*,
											 const Vector2i&,
											 const OmTileCoord&);

	void setBreakThreshold(float t){ mBreakThreshold = t; }
	float getBreakThreshold(){ return mBreakThreshold; }

	void setDustThreshold(unsigned int t){ mDustThreshold = t; }
	unsigned int getDustThreshold(){ return mDustThreshold; }

	void SetToolBarManager(ToolBarManager * tbm);
	bool GetShatterMode();
	void SetShatterMode(bool shatter);
	void ToggleShatterMode();

	std::pair<bool, SegmentDataWrapper> GetSplitMode();
	void SetSplitMode(const SegmentDataWrapper& sdw);
	void SetSplitMode(bool onoroff, bool postEvent = true);
	void SetBreakOnSplitMode(bool mode);

	void SetShowValidMode(bool mode, bool incolor);
	void SetShowSplitMode(bool mode);
	bool shouldVolumeBeShownBroken();

	void setTool(const OmToolMode tool);

	void SetHowNonSelectedSegmentsAreColoredInFilter(const bool);
	bool ShowNonSelectedSegmentsInColorInFilter();

	boost::shared_ptr<OmBrushSize>& getBrushSize(){ return brushSize_; }

	OmSegmentColorCacheType determineColorizationType(const ObjectType);

private:
	zi::mutex mColorCacheMapLock;

	MainWindow * mMainWindow;
	FilterWidget* mFilterWidget;
	boost::shared_ptr<ViewGroup> mViewGroup;
	InspectorProperties * mInspectorProperties;

	boost::shared_ptr<OmBrushSize> brushSize_;

	float mBreakThreshold;
	unsigned int mDustThreshold;

	//view event
	float mXYSlice[6], mYZSlice[6], mXZSlice[6];
	float mXYPan[2], mYZPan[2], mXZPan[2];

	boost::shared_ptr<OmZoomLevel> zoomLevel_;

	SpaceBbox mViewBbox;
	SpaceCoord mViewCenter;

	bool mXYSliceEnabled, mYZSliceEnabled, mXZSliceEnabled;

	int mViewSliceBytesPerSample;
	int mViewSliceSamplesPerPixel;
	Vector3i mViewSliceDimXY;
	Vector3i mViewSliceDimYZ;
	Vector3i mViewSliceDimXZ;

	std::vector<boost::shared_ptr<OmSegmentColorizer> > mColorCaches;

	//toolbar stuff
	ToolBarManager * mToolBarManager;
	bool mShatter;
	bool mSplitting;
	bool mBreakOnSplit;
	bool mShowValid;
	bool mShowSplit;
	bool mShowValidInColor;

	bool mShowFilterInColor;

	void setupColorizer(const Vector2i&, const OmTileCoord&,
						const OmSegmentColorCacheType);

	boost::shared_ptr<SegmentDataWrapper> segmentBeingSplit_;
};

#endif
