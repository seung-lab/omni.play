#ifndef DOM_QT_VIEW_2D_H
#define DOM_QT_VIEW_2D_H 


#include "omView2dWidget.h"
#include "omTile.h"
#include "omThreadedCachingTile.h"

#include "system/omGenericManager.h"
#include "system/events/omViewEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omSystemModeEvent.h"
#include "system/events/omPreferenceEvent.h"
#include "system/events/omVoxelEvent.h"
#include "system/omSystemTypes.h"

#include "volume/omVolumeTypes.h"

#include "common/omStd.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;

#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

#include <QGLWidget>
#include <QtGui> 

#include <QGLPixelBuffer>
#include <QGLFramebufferObject>


class OmThreadedCachingTile;
class OmTileCoord;

class OMNIView2dUpdatePlan
{
public:
        OMNIView2dUpdatePlan();

        bool operator==( const OMNIView2dUpdatePlan& rhs ) const;
        bool operator!=( const OMNIView2dUpdatePlan& rhs ) const;
};


class DOMNIView2d;

class DOMNIView2d : public QWidget,
public OmViewEventListener,
public OmSegmentEventListener,
public OmSystemModeEventListener,
public OmPreferenceEventListener,
public OmVoxelEventListener
{
        Q_OBJECT

public:
	DOMNIView2d (ViewType viewtype, ObjectType voltype, OmId image_id, OmId second_id, OmId third_id=0, QWidget *parent=0);

	QGLPixelBuffer *  pbuffer;
	OmMipVolume *mVolume;
	OmThreadedCachingTile *mCache;
	

	void wheelEvent ( QWheelEvent * event );
	void keyPressEvent (QKeyEvent *event);

	void paintEvent(QPaintEvent *event);
	void resizeEvent (QResizeEvent * event);

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
	
protected:
	void paintCrosshairs(QPainter &);
	QImage paintTextures (OMNIView2dUpdatePlan&);
	OMNIView2dUpdatePlan constructPlan ();


        // view events
        void ViewBoxChangeEvent(OmViewEvent *event);
        void ViewPosChangeEvent(OmViewEvent *event);
        void ViewCenterChangeEvent(OmViewEvent *event);
        void ViewRedrawEvent(OmViewEvent *event);

	// edit events
	void EditModeMousePressed(QMouseEvent *event);
	void EditModeMouseRelease(QMouseEvent *event);
	void EditModeMouseMove(QMouseEvent *event);


	ViewType mViewType;
	ObjectType mVolumeType;
	OmId mImageId;
	OmId mSecondId;
	OmId mThirdId;

	Vector4<int> mTotalViewport;			//lower left x, lower left y, width, height
	Vector3<int> mDataCenter;
	const DataBbox *mVolxtent;
	int mTilelen;
	int mRootLevel;
	int mNearClip;
	int mFarClip;

	bool cameraMoving;
	bool scribbling;
	Vector2<float> clickPoint;
	DataCoord lastDataPoint;
	bool drawInformation;
	bool informationUpdated;

	OMNIView2dUpdatePlan mOldPlan;

	OmId mSegmentID;
	int mSegmentValue;
};


class DOmView2d;

class DOmView2d : public QGLWidget,
public OmViewEventListener,
public OmSegmentEventListener,
public OmSystemModeEventListener,
public OmPreferenceEventListener,
public OmVoxelEventListener
{
	Q_OBJECT
	
public:
	DOmView2d(ViewType viewtype, ObjectType voltype, OmId image_id, OmId second_id, OmId third_id = 0, QWidget *parent = 0);
	
	
protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintEvent(QPaintEvent *event);	// necessary for using QPainter methods for editing segments
	void resizeEvent (QResizeEvent * event);
	
	// mouse events
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	
	void wheelEvent ( QWheelEvent * event );
	
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
	
	void ViewRedrawEvent(OmViewEvent *event);
	
	//draw methods
	void InitializeCache();
	void Draw();
	void SendFrameBuffer();
	void DrawCursors();

private:
	DataBbox SpaceToDataBbox(const SpaceBbox &spacebox);
	SpaceBbox DataToSpaceBbox(const DataBbox &databox);
	
	DataCoord SpaceToDataCoord(const SpaceCoord &spacec);
	SpaceCoord DataToSpaceCoord(const DataCoord &datac);
	
	void NavigationModeMouseDoubleClick(QMouseEvent *event);
	
	//edit mode
	void EditModeMousePressed(QMouseEvent *event);
	void EditModeMouseRelease(QMouseEvent *event);
	void EditModeMouseMove(QMouseEvent *event);
	void EditModeMouseDoubleClick(QMouseEvent *event);
	void EditModeKeyPress(QKeyEvent *event);

	
	// OmCamera2d mCamera;
	OmGenericManager< OmView2dWidget > mView2dWidgetManager;
	
	OmThreadedCachingTile *mCache;
	
	ViewType mViewType;
	ObjectType mVolumeType;
	OmId mImageId;
	OmId mSecondId;
	OmId mThirdId;
	
	// EDIT PROPERTIES
	void bresenhamLineDraw(const DataCoord &first, const DataCoord &second);

	bool iSentIt;
	
	bool scribbling;
	
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
	bool mPainting;
	
	int mNearClip;
	int mFarClip;
	
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

        friend class myQGLWidget;
};

#endif 
