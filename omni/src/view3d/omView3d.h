#ifndef OM_VIEW3D_H
#define OM_VIEW3D_H 

#include "omCamera.h"
#include "omView3dWidget.h"
#include "omView3dUi.h"

#include "system/events/omView3dEvent.h"
#include "system/events/omViewEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omPreferenceEvent.h"
#include "common/omCommon.h"

#include <QGLWidget>
#include <QTimer>
#include <QtGui> 

class OmViewGroupState;

class OmView3d : public QGLWidget, 
	public OmPreferenceEventListener,
	public OmSegmentEventListener, 
	public OmView3dEventListener, 
	public OmViewEventListener
{
	Q_OBJECT
		
  public:
	OmView3d(QWidget *, OmViewGroupState * );
	~OmView3d();
	OmCamera& GetCamera();

	
 protected:
	//gl events
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	
	//mouse events
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseWheelEvent(QWheelEvent * event);
	void wheelEvent(QWheelEvent * event );
	void keyPressEvent (QKeyEvent *);

	bool event(QEvent *e);
	void pinchTriggered(QPinchGesture *gesture);


	//omni events
	void SegmentObjectModificationEvent(OmSegmentEvent *event);
	void SegmentEditSelectionChangeEvent() {}
	void SegmentDataModificationEvent();
	void PreferenceChangeEvent(OmPreferenceEvent *event);
	void ViewBoxChangeEvent();
	void View3dRedrawEvent();
	void View3dRedrawEventFromCache();
	void View3dUpdatePreferencesEvent();
	void ViewCenterChangeEvent() {}
	void ViewPosChangeEvent() {}
	void ViewRedrawEvent() {}
		
	//edit actions
	void SelectSegment(QMouseEvent *event);
	
	//voxel selection
	void FocusSelectVoxel(QKeyEvent *event);
	void MouseSelectVoxel(QMouseEvent *event, bool drag);
	void SelectVoxel(DataCoord &voxel, bool append);
		
	//gl actions
	bool PickPoint(Vector2<int> point, vector<unsigned int> &names);
	bool UnprojectPoint(Vector2i point2d, Vector3f &point3d, float depth_scale_factor = 1.0f);

	//draw methods
	void Draw(OmBitfield option);
	void DrawVolumes(OmBitfield option);
	void DrawEditSelectionVoxels();
	void DrawWidgets();
	void doTimedDraw();
	void myUpdate();
		
	//draw settings
	void SetBackgroundColor();
	void SetBlending();
	void SetCameraPerspective();
		
	//widgets
	void UpdateEnabledWidgets();
		
	QSize sizeHint () const;
		
 private:
	bool gestureEvent(QGestureEvent *event);
	OmView3dUi mView3dUi;
	OmViewGroupState * mViewGroupState;
	QTime * mElapsed;
	QTimer mDrawTimer;
	OmCamera mCamera;
	std::vector< OmView3dWidget* > mView3dWidgetManager;
		
	vector<int> mMousePickResult;	
		
	friend class OmView3dUi;
	friend class OmSelectionWidget;
	friend class OmChunkExtentWidget;
};

#endif 
