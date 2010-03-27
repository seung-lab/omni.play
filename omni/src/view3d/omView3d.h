#ifndef OM_VIEW3D_H
#define OM_VIEW3D_H 

#include "omCamera.h"
#include "omView3dWidget.h"
#include "omView3dUi.h"

#include "volume/omVolumeTypes.h"
#include "system/omGenericManager.h"

#include "system/events/omView3dEvent.h"
#include "system/events/omViewEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omVoxelEvent.h"
#include "system/events/omSystemModeEvent.h"
#include "system/events/omPreferenceEvent.h"
#include "common/omStd.h"
#include <boost/progress.hpp>
#include <vmmlib/vmmlib.h>
using namespace vmml;

#include <QGLWidget>
#include <QTimer>
#include <QtGui> 

class OmView3d : public QGLWidget, 
	public OmPreferenceEventListener,
	public OmSegmentEventListener, 
	public OmVoxelEventListener,
	public OmSystemModeEventListener,
	public OmView3dEventListener, 
	public OmViewEventListener
{
	Q_OBJECT
		
		public:
	OmView3d(QWidget *parent);
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

	//omni events
	void SegmentObjectModificationEvent(OmSegmentEvent *event);
	void VoxelModificationEvent(OmVoxelEvent *event);
	void SegmentDataModificationEvent(OmSegmentEvent *event);
	void PreferenceChangeEvent(OmPreferenceEvent *event);
	void SystemModeChangeEvent(OmSystemModeEvent *event);
	void ViewBoxChangeEvent(OmViewEvent *event);
	//void ViewCenterChangeEvent(OmViewEvent *event);
	void View3dRedrawEvent(OmView3dEvent *event);
	void View3dRedrawEventFromCache(OmView3dEvent * event);
	void View3dUpdatePreferencesEvent(OmView3dEvent *event);
		
		
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
	boost::timer * mElapsed;
	QTimer mDrawTimer;
	OmView3dUi mView3dUi;
	OmCamera mCamera;
	OmGenericManager< OmView3dWidget > mView3dWidgetManager;
		
	vector<int> mMousePickResult;
		
		
	friend class OmView3dUi;
	friend class OmSelectionWidget;
	friend class OmChunkExtentWidget;
};

#endif 
