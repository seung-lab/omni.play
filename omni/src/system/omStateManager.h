#ifndef OM_STATE_MANAGER_H
#define OM_STATE_MANAGER_H

/*
 *	Manages data structures that are shared between various parts of the system.  Making centralized
 *	changes in the StateManager will send events that cause the other interested systems to be
 *	notified and synchronized.
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "omSystemTypes.h"
#include "volume/omVolumeTypes.h"
#include "common/omSerialization.h"
#include "common/omStd.h"

#include <QString>

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;


class QUndoStack;
class QUndoCommand;
class QGLWidget;
class QGLContext;



enum OmSlicePlane { SLICE_XY_PLANE, SLICE_XZ_PLANE, SLICE_YZ_PLANE };
enum OmSystemMode { NAVIGATION_SYSTEM_MODE, EDIT_SYSTEM_MODE };
enum OmToolMode { SELECT_MODE,
		  PAN_MODE,
		  CROSSHAIR_MODE,
		  STICKY_CROSSHAIR_MODE,
		  ZOOM_MODE, 
		  ADD_VOXEL_MODE, 
		  SUBTRACT_VOXEL_MODE, 
		  SELECT_VOXEL_MODE,
		  FILL_MODE, 
		  VOXELIZE_MODE,
};


class OmStateManager {

public:
	
	static OmStateManager* Instance();
	static void Delete();
	

	//project
	static const string& GetProjectFileName();
	static void SetProjectFileName(const string &);
	static const string& GetProjectDirectoryPath();
	static void SetProjectDirectoryPath(const string &);
	
	
	//viewbox state
	static void SetViewSliceMin(ViewType, Vector2<float>, bool postEvent = true);
	static Vector2<float> GetViewSliceMin(ViewType);
	
	static void SetViewSliceMax(ViewType, Vector2<float>, bool postEvent = true);
	static Vector2<float> GetViewSliceMax(ViewType);
	
	static void SetViewSliceDepth(ViewType, float, bool postEvent = true);
	static float GetViewSliceDepth(ViewType);
	
	static void SetZoomLevel(Vector2<int>);
	static Vector2<int> GetZoomLevel();
	
	static void SetPanDistance(ViewType, Vector2<int>, bool postEvent = true);
	static Vector2<int> GetPanDistance(ViewType);
	
	
	// slices
	static void SetSliceState(OmSlicePlane plane, bool enabled);

	static void SetViewSliceDataFormat(int bytesPerSample, int samplesPerPixel);
	static void SetViewSlice(const OmSlicePlane plane, const Vector3<int> &dim, unsigned char *data);

	
	
	//system mode
	static OmSystemMode GetSystemMode();
	static void SetSystemMode(const OmSystemMode mode);
	
	
	//tool mode
	static OmToolMode GetToolMode();
	static void SetToolMode(const OmToolMode mode);
	
	
	//undostack
	static QUndoStack* GetUndoStack();
	static void PushUndoCommand(QUndoCommand *);
	static void ClearUndoStack();
	
	
	//view3d context
	static void CreatePrimaryView3dWidget();
	static const QGLWidget* GetPrimaryView3dWidget();
	static QGLContext* GetSharedView3dContext();
	static void MakeContextCurrent(QGLContext* context);
	
	//view2d context
	static QGLContext* GetSharedView2dContext(const QGLContext *pContext);
	
	//central transparency value (to avoid gross pref lookups)
	static float GetTransparencyAlpha();
	static void SetTransparencyAlpha(float);
	
	static unsigned int getMyBackoff();
	static void setMyBackoff( unsigned int val );

	static void setOmniExecutableAbsolutePath( QString abs_path );
	static QString getOmniExecutableAbsolutePath();

protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmStateManager();
	~OmStateManager();
	OmStateManager(const OmStateManager&);
	OmStateManager& operator= (const OmStateManager&);

	
private:
	//singleton
	static OmStateManager* mspInstance;
	
	//project
	string mProjectFileName;
	string mProjectDirectoryPath;
	
	//view event
	float mXYSlice[6], mYZSlice[6], mXZSlice[6];
	int mXYPan[2], mYZPan[2], mXZPan[2];
	Vector2<int> zoom_level;
	
	SpaceBbox mViewBbox;
	SpaceCoord mViewCenter;
	
	bool mXYSliceEnabled, mYZSliceEnabled, mXZSliceEnabled;
	
	int mViewSliceBytesPerSample, mViewSliceSamplesPerPixel;
	Vector3<int> mViewSliceDimXY, mViewSliceDimYZ, mViewSliceDimXZ;
	unsigned char *mViewSliceDataXY, *mViewSliceDataYZ, *mViewSliceDataXZ;
	
	
	//edit selection
	Vector3< OmId > mEditSelection;	//volume, segmentation, segment
	set< DataCoord > mEditSelectionCoords;
	
	//system mode
	OmSystemMode mSystemMode;
	
	//tool mode
	OmToolMode mToolMode;
	
	//undostack
	QUndoStack *mpUndoStack;
	
	//view3d context
	QGLWidget *mpPrimaryView3dWidget;
	
	//transparency
	float TRANSPARANCY_ALPHA;

	unsigned int myBackoff;

	QString omniExecPathAbsolute;
	/*
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
	 */
};





	
#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization

/*
BOOST_CLASS_VERSION(OmStateManager, 0)

template<class Archive>
void 
OmStateManager::serialize(Archive & ar, const unsigned int file_version) {
	ar & mViewBbox;
	ar & mViewCenter;
}
*/


#endif
