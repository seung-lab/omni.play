#ifndef OM_QT_VIEW_2D_H
#define OM_QT_VIEW_2D_H 

#include "drawable.h"

#include <QGLPixelBuffer>

#include "omView2dWidget.h"
#include "omTile.h"
#include "omThreadedCachingTile.h"

#include "volume/omMipChunkCoord.h"
#include "volume/omMipChunk.h"

#include "system/omGenericManager.h"
#include "system/events/omViewEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omSystemModeEvent.h"
#include "system/events/omPreferenceEvent.h"
#include "system/events/omVoxelEvent.h"
#include "system/omSystemTypes.h"
#include "volume/omFilter2d.h"

#include "volume/omVolumeTypes.h"
#include "volume/omSegmentation.h"

#include "common/omStd.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;

#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;
#include <boost/progress.hpp>

#include <QGLWidget>
#include <QtGui> 

class OmThreadedCachingTile;
class OmTileCoord;

class OmView2d : public QWidget,
public OmViewEventListener,
public OmSegmentEventListener,
public OmSystemModeEventListener,
public OmPreferenceEventListener,
public OmVoxelEventListener
{
	Q_OBJECT
	
public:
	OmView2d(ViewType viewtype, ObjectType voltype, OmId image_id, QWidget *parent);
 	~OmView2d();
	
	OmMipVolume *mVolume;
	OmSegmentation *mSeg;
	QGLPixelBuffer *  pbuffer;

	int GetDepthToDataSlice(ViewType viewType);
	void SetDataSliceToDepth(ViewType viewType, int slice);
	int GetDepthToDataMax(ViewType viewType);
	
protected:
	// GL event methods
	void initializeGL();
	void resizeGL(int width, int height);
	void resizeEvent (QResizeEvent * event);
	void PanAndZoom(Vector2<int> new_zoom, bool postEvent = true);	// Helper for zooming.
	void SetViewSliceOnPan ();						// Helper for panning.

	// paint and draw methods
	void paintEvent(QPaintEvent *event);	// necessary for using QPainter methods for editing segments
	QImage safePaintEvent(QPaintEvent *event);	// pbuffered paint.
	void Draw(int mip=true);
	void TextureDraw(vector <Drawable*> &textures);
	void safeTexture(shared_ptr<OmTextureID> gotten_id);
	void safeDraw(float zoomFactor, int x, int y, int tileLength, shared_ptr<OmTextureID> gotten_id);
	void PreDraw(Vector2f);
	bool BufferTiles(Vector2f zoomMipVector, bool buffertiles);


	void DrawFromFilter (OmFilter2d&);
	void DrawFromCache();

	void SendFrameBuffer(QImage * img);
	void DrawCursors();

	// Various Category methods
	void setBrushToolDiameter();
	DataCoord BrushToolOTGDC(DataCoord off);
	void PickToolGetColor(QMouseEvent *event);
	void PickToolAddToSelection (OmId segmentation_id, DataCoord globalDataClickPoint);
	void FillToolFill (OmId segmentation, DataCoord gCP, SEGMENT_DATA_TYPE fc, SEGMENT_DATA_TYPE bc, int depth=0);
	void BrushToolApplyPaint(OmId segid, DataCoord gDC, SEGMENT_DATA_TYPE seg);

	// EDIT PROPERTIES
	void bresenhamLineDraw(const DataCoord &first, const DataCoord &second);
	void myUpdate ();
	void Refresh ();

	// Possibly Obsolete methods
	void* GetImageData(const OmTileCoord &key, Vector2<int> &sliceDims, OmMipVolume *vol);
	OmIds setMyColorMap(OmId segmentation, SEGMENT_DATA_TYPE *imageData, Vector2<int> dims, const OmTileCoord &key, void **rData);
	int GetDepth(const OmTileCoord &key, OmMipVolume *vol);
	void myBindToTextureID(boost::shared_ptr<OmTextureID>);

	///////////////////////////////////////
	// OmView2dEvent.cpp:
	///////////////////////////////////////

	// mouse events
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	
	// event
	void mouseNavModeLeftButton(QMouseEvent *event);
	void mouseEditModeLeftButton(QMouseEvent *event);
	void mouseSelectSegment(QMouseEvent *event);
	void mouseSetCrosshair(QMouseEvent *event);
	void wheelEvent ( QWheelEvent * event );
	void MoveUpStackCloserToViewer();
	void MoveDownStackFartherFromViewer();
	void MouseWheelZoom( const int numSteps );

	void mouseZoomIn();
	void mouseZoomOut();
	void mouseZoom(QMouseEvent *event);	
	void ViewRedrawEvent(OmViewEvent *event);
	void NavigationModeMouseDoubleClick(QMouseEvent *event);

	//edit mode
	void EditModeMouseRelease(QMouseEvent *event);
	void EditModeMouseMove(QMouseEvent *event);
	void EditModeMouseDoubleClick(QMouseEvent *event);
	void EditModeKeyPress(QKeyEvent *event);	
	void EditMode_MouseMove_LeftButton_Scribbling(QMouseEvent *event);
	void EditMode_MouseRelease_LeftButton_Filling(QMouseEvent *event);
	void mouseMove_NavMode_CamMoving(QMouseEvent *event);
	void EditMode_MousePressed_LeftButton(QMouseEvent *event);
	void SetDepth(QMouseEvent *event);
	DataCoord getMouseClickpointLocalDataCoord( QMouseEvent *event, const ViewType viewType = XY_VIEW );
	DataCoord getMouseClickpointGlobalDataCoord( QMouseEvent *event);

	// key events
	void keyPressEvent (QKeyEvent *event);
	
	// omni events
	void PreferenceChangeEvent(OmPreferenceEvent *event);
	void SegmentObjectModificationEvent(OmSegmentEvent *event);
	void SegmentDataModificationEvent(OmSegmentEvent *event);
	
	// Change to edit selection
	void SegmentEditSelectionChangeEvent(OmSegmentEvent *event);
	void SystemModeChangeEvent(OmSystemModeEvent *event);
	void VoxelModificationEvent(OmVoxelEvent *event);
	
	// view events
	void ViewBoxChangeEvent(OmViewEvent *event);
	void ViewPosChangeEvent(OmViewEvent *event);
	void ViewCenterChangeEvent(OmViewEvent *event);
	///////////////////////////////////////

	QSize sizeHint () const;

private:
	///////////////////////////////////////
	// omView2dConverters.cpp
	///////////////////////////////////////
	DataBbox SpaceToDataBbox(const SpaceBbox &spacebox);
	SpaceBbox DataToSpaceBbox(const DataBbox &databox);
	DataCoord SpaceToDataCoord(const SpaceCoord &spacec);
	SpaceCoord DataToSpaceCoord(const DataCoord &datac);
	Vector2f ScreenToPanShift(Vector2i screenshift);
	SpaceCoord ScreenToSpaceCoord(ViewType viewType,const ScreenCoord &screenc);
	ScreenCoord SpaceToScreenCoord(ViewType viewType,const SpaceCoord &spacec);
	ScreenCoord DataToScreenCoord(ViewType viewType,const DataCoord &datac);
	DataCoord ScreenToDataCoord(ViewType viewType,const ScreenCoord &screenc);
	NormCoord ScreenToNormCoord(ViewType viewType,const ScreenCoord &screenc);
        DataCoord ToDataCoord(int xMipChunk, int yMipChunk, int mDataDepth);
	Vector2f GetPanDistance(ViewType viewType);
	///////////////////////////////////////


	// Global variables
	OmId mEditedSegmentation;
	bool mMIP;
        unsigned int mSlide;
        bool mEmitMovie;
	float mDragX, mDragY;
	bool mDoRefresh;
	int mBrushToolMaxX, mBrushToolMaxY, mBrushToolMaxZ, mBrushToolMinX, mBrushToolMinY, mBrushToolMinZ;
	boost::timer* mElapsed;
	OmId mCurrentSegmentId;
	set<DataCoord> mUpdateCoordsSet; 
	int mBrushToolDiameter;
	OmId mCurrentSegmentation;
	QImage mImage;
	QPainter painter;
	bool mLevelLock;
	bool mNewDraw;
	vector <Drawable*> mTextures;
	bool mHaveFirstSegId;
	bool mHaveSecondSegId;
	OmId mFirstSegId;
	OmId mSecondSegId;
	
	int mTileCount;
	int mTileCountIncomplete;
        int mBufferTileCount;
        int mBufferTileCountIncomplete;

	Vector2i mMousePoint;
	
	// OmCamera2d mCamera;
	OmGenericManager< OmView2dWidget > mView2dWidgetManager;
	
	OmThreadedCachingTile *mCache;
	double mAlpha;
	bool mJoiningSegmentToggled;
	
	ViewType mViewType;
	ObjectType mVolumeType;
	OmId mImageId;
	bool iSentIt;
	bool mScribbling;
	bool mInitialized;
	
	// FLAT data coordinates, not accurate for orthogonal views but accurate for Bresenham
	DataCoord lastDataPoint;
	
	QColor editColor;
	
	OmIds modified_Ids;
	bool delete_dirty;
	
	// slice props - spatial coords
	
	// corresponds to window viewport - is not modified except through window resizes
	Vector4i mTotalViewport;			//lower left x, lower left y, width, height
	
	// corresponds to slice viewport - is not modified except through window resizes
	Vector4i mLocalViewport;
	
	// cursor props
	ScreenCoord clickPoint;
	SpaceCoord rememberCoord;
	SpaceCoord rememberDepthCoord;

	// These coords are accurate for orthogonal views, not flat coordinates.
	set<DataCoord> modifiedCoords;
	
	OmId mSegmentID;
	int mSegmentValue;
	
	bool cameraMoving;
	bool firstResize;
	
	int mNearClip;
	int mFarClip;
	
	Vector2i mMin;
	Vector2i mMax;
	
	int mZoomLevel;
	
	int mRootLevel;
	
	bool firstPaintEvent;
	
	int depthCache;
	int sidesCache;
	int mipCache;
	
	bool drawComplete;
	bool sentTexture;

	bool amInFillMode();
	bool doDisplayInformation();

	void displayInformation( QString & elapsedTime );
};

#endif 
