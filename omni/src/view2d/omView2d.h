#ifndef OM_QT_VIEW_2D_H
#define OM_QT_VIEW_2D_H 

#include <QGLFramebufferObject>
#include <QGLPixelBuffer>
#include <QGLWidget>
#include <QPainter>

#include "system/events/omPreferenceEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omSystemModeEvent.h"
#include "system/events/omViewEvent.h"
#include "system/events/omVoxelEvent.h"
#include "view2d/omView2dImpl.h"
#include "gui/omSegmentContextMenu.h"

class Drawable;
class OmTileCoord;
class SegmentDataWrapper;

class OmMipVolume;
class OmTextureID;
class OmSegmentation;
class OmFilter2d;

class OmView2d : 
	public OmView2dImpl,
	public OmPreferenceEventListener,
	public OmSegmentEventListener,
	public OmSystemModeEventListener,
	public OmViewEventListener,
	public OmVoxelEventListener
	
{
	Q_OBJECT
	
public:
	OmView2d(ViewType, ObjectType, OmId, QWidget *, OmViewGroupState *);
 	~OmView2d();
	

	OmSegmentation *mSeg;


	int GetDepthToDataSlice(ViewType viewType);
	void SetDataSliceToDepth(ViewType viewType, int slice);
	int GetDepthToDataMax(ViewType viewType);

protected:
	// GL event methods

	void resizeGL(int width, int height);
	void resizeEvent (QResizeEvent * event);
	void PanAndZoom(Vector2<int> new_zoom, bool postEvent = true);	// Helper for zooming.
	void SetViewSliceOnPan ();						// Helper for panning.

	// paint and draw methods
	void paintEvent(QPaintEvent *);	// necessary for using QPainter methods for editing segments

	void DrawCursors();

	// Various Category methods
	void setBrushToolDiameter();
	DataCoord BrushToolOTGDC(DataCoord off);

	void PickToolAddToSelection (OmId segmentation_id, DataCoord globalDataClickPoint);
	void FillToolFill (OmId segmentation, DataCoord gCP, OmSegID fc, OmSegID bc, int depth=0);
	void BrushToolApplyPaint(OmId segid, DataCoord gDC, OmSegID seg);

	// EDIT PROPERTIES
	void bresenhamLineDraw(const DataCoord &first, const DataCoord &second, bool doselection = false);
	void myUpdate ();
	void Refresh ();

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
	void ViewRedrawEvent();
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
	DataCoord getMouseClickpointLocalDataCoord( QMouseEvent *event );
	DataCoord getMouseClickpointGlobalDataCoord( QMouseEvent *event);

	// key events
	void keyPressEvent (QKeyEvent *event);
	
	// omni events
	void PreferenceChangeEvent(OmPreferenceEvent *event);
	void SegmentObjectModificationEvent(OmSegmentEvent*);
	void SegmentDataModificationEvent();
	
	// Change to edit selection
	void SegmentEditSelectionChangeEvent();
	void SystemModeChangeEvent();
	void VoxelModificationEvent(OmVoxelEvent *event);
	void VoxelSelectionModificationEvent() {}

	// view events
	void ViewBoxChangeEvent();
	void ViewPosChangeEvent();
	void ViewCenterChangeEvent();
	///////////////////////////////////////

	QSize sizeHint () const;

private:


	///////////////////////////////////////
	// omView2dConverters.cpp
	///////////////////////////////////////
	DataBbox SpaceToDataBbox(const SpaceBbox &spacebox);
	SpaceBbox DataToSpaceBbox(const DataBbox &databox);

	Vector2f ScreenToPanShift(Vector2i screenshift);
	SpaceCoord ScreenToSpaceCoord( const ScreenCoord &screenc);
	ScreenCoord SpaceToScreenCoord(ViewType viewType,const SpaceCoord &spacec);
	ScreenCoord DataToScreenCoord(ViewType viewType,const DataCoord &datac);
	DataCoord ScreenToDataCoord( const ScreenCoord &screenc);
	NormCoord ScreenToNormCoord(ViewType viewType,const ScreenCoord &screenc);

	OmId mEditedSegmentation;
	bool mMIP;
        unsigned int mSlide;
        bool mEmitMovie;
	float mDragX, mDragY;
	bool mDoRefresh;
	int mBrushToolMaxX, mBrushToolMaxY, mBrushToolMaxZ, mBrushToolMinX, mBrushToolMinY, mBrushToolMinZ;
	QTime * mElapsed;
	OmId mCurrentSegmentId;
	set<DataCoord> mUpdateCoordsSet; 
	int mBrushToolDiameter;
	QImage mImage;
	QPainter painter;
	bool mLevelLock;
	bool mNewDraw;

	bool mHaveFirstSegId;
	bool mHaveSecondSegId;
	OmId mFirstSegId;
	OmId mSecondSegId;

        int mBufferTileCount;
        int mBufferTileCountIncomplete;

	Vector2i mMousePoint;

	bool mJoiningSegmentToggled;

	bool iSentIt;

	bool mInitialized;
	
	// FLAT data coordinates, not accurate for orthogonal views but accurate for Bresenham
	DataCoord lastDataPoint;
	
	QColor editColor;
	
	OmIDsSet modified_Ids;
	bool delete_dirty;
	
	// slice props - spatial coords
	
	// corresponds to window viewport - is not modified except through window resizes

	
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
	
	Vector2i mMin;
	Vector2i mMax;
	
	bool firstPaintEvent;
	
	int depthCache;
	int sidesCache;
	int mipCache;

	bool sentTexture;

	bool amInFillMode();
	bool doDisplayInformation();

	void displayInformation( QString & elapsedTime );

	void doSelectSegment( SegmentDataWrapper sdw, bool augment_selection );
	void resetWindow();
	void resetWindowState();
	void doFindAndSplitSegment(QMouseEvent * event );
	SegmentDataWrapper * getSelectedSegment( QMouseEvent * event );

	void doRedraw();

	OmSegmentContextMenu mSegmentContextMenu;
	void mouseShowSegmentContextMenu(QMouseEvent * event);


#ifdef WIN32
	typedef void (*GLCOLOR)(GLfloat, GLfloat, GLfloat, GLfloat);
	GLCOLOR mGlBlendColorFunction;
#endif
};

#endif 
