#ifndef OM_QT_VIEW_2D_H
#define OM_QT_VIEW_2D_H 

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
#include "system/omFilter2d.h"

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


class Drawable {
public:
        int x, y, tileLength;
	float zoomFactor;
        shared_ptr<OmTextureID> gotten_id;
	OmTileCoord tileCoord;
	bool mGood;
        Drawable (int x, int y, int tileLength, OmTileCoord tileCoord, float zoomFactor, shared_ptr<OmTextureID> gotten_id);
        Drawable (int x, int y, int tileLength, OmTileCoord tileCoord, float zoomFactor);
};


class OmView2d : public QWidget,
public OmViewEventListener,
public OmSegmentEventListener,
public OmSystemModeEventListener,
public OmPreferenceEventListener,
public OmVoxelEventListener
{
	Q_OBJECT
	
public:
	OmView2d(ViewType viewtype, ObjectType voltype, OmId image_id, OmId second_id, OmId third_id = 0, QWidget *parent = 0);
	// OmView2d(ViewType viewtype, ObjectType voltype, OmId image_id, OmId second_id, OmId third_id, QWidget *parent = 0);
	// OmCamera2d& GetCamera();
 	~OmView2d();
	
	OmMipVolume *mVolume;
	OmSegmentation *mSeg;
	QGLPixelBuffer *  pbuffer;

	
	
protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void resizeEvent (QResizeEvent * event);
        DataCoord ToDataCoord(int xMipChunk, int yMipChunk, int mDataDepth);

	void PanOnZoom(Vector2<int> current_zoom, bool postEvent = true);	// Helper for zooming.
	void SetViewSliceOnPan ();						// Helper for panning.
	void GlobalDepthFix (float howMuch);					// Helper for zooming.
	void PanOnZoomSelf (Vector2<int> current_zoom);				// Helper for mipping.


	

	// void paintGL();
	void paintEvent(QPaintEvent *event);	// necessary for using QPainter methods for editing segments
	QImage safePaintEvent(QPaintEvent *event);	// pbuffered paint.
	
	void PickToolGetColor(QMouseEvent *event);
	void PickToolAddToSelection (OmId segmentation_id, DataCoord globalDataClickPoint);

	void FillToolFill (OmId segmentation, DataCoord gCP, SEGMENT_DATA_TYPE fc, SEGMENT_DATA_TYPE bc, int depth=0);
	void BrushToolApplyPaint(OmId segid, DataCoord gDC, SEGMENT_DATA_TYPE seg);


	void myUpdate ();
	void Refresh ();


	// mouse events
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	
	void wheelEvent ( QWheelEvent * event );
	
	// key events
	void keyPressEvent (QKeyEvent *event);
	
	// omni events
	// void View3dRedrawEvent(OmView3dEvent *event);
	// void View3dUpdatePreferencesEvent(OmView3dEvent *event);
	void PreferenceChangeEvent(OmPreferenceEvent *event);
	
	void SegmentObjectModificationEvent(OmSegmentEvent *event);
	void SegmentDataModificationEvent(OmSegmentEvent *event);
	
	// Change to edit selection
	void SegmentEditSelectionChangeEvent(OmSegmentEvent *event);
	
	void SystemModeChangeEvent(OmSystemModeEvent *event);
	
	void VoxelModificationEvent(OmVoxelEvent *event);
	
	//edit actions
	// void SelectSegment(QMouseEvent *event);
	
	// view events
	void ViewBoxChangeEvent(OmViewEvent *event);
	void ViewPosChangeEvent(OmViewEvent *event);
	void ViewCenterChangeEvent(OmViewEvent *event);
	
	void ViewRedrawEvent(OmViewEvent *event);
	
	//draw methods
	void Draw(int mip=true);
	void Draw(vector <Drawable*> &textures);
	void safeTexture(shared_ptr<OmTextureID> gotten_id);
	void safeDraw(float zoomFactor, int x, int y, int tileLength, shared_ptr<OmTextureID> gotten_id);
	void PreDraw(Vector2i);


	void DrawFromFilter (OmFilter2d&);
	void DrawFromCache();


	void SendFrameBuffer(QImage * img);
	void DrawCursors();
	// void DrawEditSelection();
	
	//actions
	// vector<int> PickPoint(Vector2<int> pt, OmBitfield drawOptions);
	
	void* GetImageData(const OmTileCoord &key, Vector2<int> &sliceDims, OmMipVolume *vol);
	OmIds setMyColorMap(OmId segmentation, SEGMENT_DATA_TYPE *imageData, Vector2<int> dims, const OmTileCoord &key, void **rData);
	int GetDepth(const OmTileCoord &key, OmMipVolume *vol);
	void myBindToTextureID(boost::shared_ptr<OmTextureID>);

	DataCoord BrushToolOTGDC(DataCoord off);



private:
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
	bool mLevelLock;
	bool mNewDraw;
	vector <Drawable*> mTextures;
	bool mHaveFirstSegId;
	bool mHaveSecondSegId;
	OmId mFirstSegId;
	OmId mSecondSegId;
	
	int mTileCount;
	Vector2f mMousePoint;

	// NormCoord DataToNormCoord(const DataCoord &data, bool centered);
	DataBbox SpaceToDataBbox(const SpaceBbox &spacebox);
	SpaceBbox DataToSpaceBbox(const DataBbox &databox);
	
	DataCoord SpaceToDataCoord(const SpaceCoord &spacec);
	SpaceCoord DataToSpaceCoord(const DataCoord &datac);
	
	void NavigationModeMouseDoubleClick(QMouseEvent *event);
	// void EditModeMouseDoubleClick(QMouseEvent *event);
	
	//edit mode
	void EditModeMouseRelease(QMouseEvent *event);
	void EditModeMouseMove(QMouseEvent *event);
	void EditModeMouseDoubleClick(QMouseEvent *event);
	void EditModeKeyPress(QKeyEvent *event);

	
	// OmCamera2d mCamera;
	OmGenericManager< OmView2dWidget > mView2dWidgetManager;
	
	//		OmCachingTile *mCache;
	OmThreadedCachingTile *mCache;
	double mAlpha;
	bool mJoiningSegmentToggled;
	
//	OmView2dUi mView2dUi;
	
	ViewType mViewType;
	ObjectType mVolumeType;
	OmId mImageId;
	OmId mSecondId;
	OmId mThirdId;
	
	// EDIT PROPERTIES
	void bresenhamLineDraw(const DataCoord &first, const DataCoord &second);

	bool iSentIt;
	bool mScribbling;
	
	// FLAT data coordinates, not accurate for orthogonal views but accurate for Bresenham
	DataCoord lastDataPoint;
	
	QColor editColor;
	
	OmIds modified_Ids;
	bool delete_dirty;
	
	// slice props - spatial coords
	
	// corresponds to window viewport - is not modified except through window resizes
	Vector4<int> mTotalViewport;			//lower left x, lower left y, width, height
	
	// corresponds to slice viewport - is not modified except through window resizes
	Vector4<int> mLocalViewport;
	
	
	// cursor props
	Vector2<float> clickPoint;
	
	// These coords are accurate for orthogonal views, not flat coordinates.
	set<DataCoord> modifiedCoords;
	
	OmId mSegmentID;
	int mSegmentValue;
	
	bool cameraMoving;
	bool firstResize;
	
	int mNearClip;
	int mFarClip;
	
	//		int widthTranslate;
	//		int heightTranslate;
	//		
	//		float scaleFactor;
	
	Vector2i mMin;
	Vector2i mMax;
	
	int mZoomLevel;
	
	int mRootLevel;
	
	bool drawInformation;
	bool firstPaintEvent;
	bool informationUpdated;
	
	int depthCache;
	int sidesCache;
	int mipCache;
	
	bool drawComplete;
	bool sentTexture;

	void MoveUpStackCloserToViewer();
	void MoveDownStackFartherFromViewer();
	void MouseWheelZoom( const int numSteps );
	void setBrushToolDiameter();

	void EditMode_MouseMove_LeftButton_Scribbling(QMouseEvent *event);
	void EditMode_MouseRelease_LeftButton_Filling(QMouseEvent *event);
	void mouseMove_NavMode_CamMoving(QMouseEvent *event);
	void mouseMove_NavMode_DrawInfo(QMouseEvent *event);	
	void EditMode_MousePressed_LeftButton(QMouseEvent *event);
	void mouseDoubleClickEvent_SetDepth(QMouseEvent *event);
	DataCoord getMouseClickpointLocalDataCoord( QMouseEvent *event, const ViewType viewType = XY_VIEW );
	DataCoord getMouseClickpointGlobalDataCoord( QMouseEvent *event);

	void mouseNavModeLeftButton(QMouseEvent *event);
	void mouseEditModeLeftButton(QMouseEvent *event);
	void mouseSelectSegment(QMouseEvent *event);
	void mouseSetCrosshair(QMouseEvent *event);
	void mouseZoomIn();
	void mouseZoomOut();
	void mouseZoom(QMouseEvent *event);
	bool amInFillMode();


};

#endif 
