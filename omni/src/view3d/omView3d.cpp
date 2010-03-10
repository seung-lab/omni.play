#include "omView3d.h"

#include "widgets/omInfoWidget.h"
#include "widgets/omSelectionWidget.h"
#include "widgets/omViewBoxWidget.h"
#include "widgets/omChunkExtentWidget.h"

#include "segment/omSegmentEditor.h"
#include "segment/actions/segment/omSegmentSelectionAction.h"

#include "volume/omVolumeCuller.h"

#include "system/omKeyManager.h"
#include "system/omStateManager.h"
#include "project/omProject.h"
#include "system/omSystemTypes.h"
#include "system/omEventManager.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "common/omGl.h"
#include "common/omDebug.h"

enum View3dWidgetIds {
	VIEW3D_WIDGET_ID_SELECTION = 1,
	VIEW3D_WIDGET_ID_VIEWBOX,
	VIEW3D_WIDGET_ID_INFO,
	VIEW3D_WIDGET_ID_CHUNK_EXTENT
};

/////////////////////////////////////////
///////         Utility Function Prototypes

void initLights();

/////////////////////////////////////
//////////
//////////      OmView3d Class
//////////

static int secretBackoff = 0;

/*
 *	Constructs View3d widget that shares with the primary widget.
 */
OmView3d::OmView3d(QWidget * parent)
 : QGLWidget(parent, OmStateManager::GetPrimaryView3dWidget()), mView3dUi(this)
{
	//set keyboard policy
	setFocusPolicy(Qt::ClickFocus);

	//setup widgets
	mView3dWidgetManager.Set(VIEW3D_WIDGET_ID_SELECTION, new OmSelectionWidget(this));
	mView3dWidgetManager.Set(VIEW3D_WIDGET_ID_VIEWBOX, new OmViewBoxWidget(this));
	mView3dWidgetManager.Set(VIEW3D_WIDGET_ID_INFO, new OmInfoWidget(this));
	mView3dWidgetManager.Set(VIEW3D_WIDGET_ID_CHUNK_EXTENT, new OmChunkExtentWidget(this));

	//update enabled state of widgets
	UpdateEnabledWidgets();

	OmStateManager::setMyBackoff( 1 );
}


static void resetBackoff ()
{
	OmStateManager::setMyBackoff( 1 );
	secretBackoff = 0;
}

static void increaseBackoff ()
{
	secretBackoff++;
	if (secretBackoff > 50) {
		OmStateManager::setMyBackoff( OmStateManager::getMyBackoff() + 1 );
		secretBackoff = 0;
	}
}

/////////////////////////////////
///////          Accessor Methods

OmCamera & OmView3d::GetCamera()
{
	return mCamera;
}

/////////////////////////////////
///////          GL Event Methods

void OmView3d::initializeGL()
// The initializeGL() function is called just once, before paintGL() is called.
{

	glShadeModel(GL_SMOOTH);	// shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	// 4-byte pixel alignment

	// enable /disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);	// normalize normals for lighting
	//glEnable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE);	// enable culling
	glCullFace(GL_BACK);	// specify backface culling

	//set material properties
	glEnable(GL_COLOR_MATERIAL);	// cause material to track current color
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	// cause ambient and diffust to track color

	float black[4] = { 0, 0, 0, 0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, black);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);

	//set drawing properties

	SetBackgroundColor();	// background color
	glClearStencil(0);	// clear stencil buffer
	glClearDepth(1.0f);	// 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);	// drawn if depth value is less than or equal
	// than previous stored depth value

	SetBlending();
	//glEnable(GL_BLEND);                                                   // enable blending for transparency
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);            // set blend function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);

	initLights();

	Vector4 < int >viewport(0, 0, 400, 300);
	mCamera.SetViewport(viewport);	//set viewport

	SetCameraPerspective();	// camera props
}

/*
 *	Window resize event
 */
void OmView3d::resizeGL(int width, int height)
{
	mCamera.ApplyReshape(Vector2 < int >(width, height));
}

/*
 *	Paint window event.
 */
void OmView3d::paintGL()
{
	Draw(DRAWOP_LEVEL_ALL | DRAWOP_RENDERMODE_RENDER | DRAWOP_DRAW_WIDGETS);
	//debug("FIXME", << "Done THREE D drawing" << endl;
}

/////////////////////////////////
///////          QEvent Methods

void OmView3d::mousePressEvent(QMouseEvent * event)
{
	resetBackoff();
	try {
		mView3dUi.MousePressed(event);
	} catch(...) {

	}
}

void OmView3d::mouseReleaseEvent(QMouseEvent * event)
{
	resetBackoff();
	mView3dUi.MouseRelease(event);
}

void OmView3d::mouseMoveEvent(QMouseEvent * event)
{
	resetBackoff();
	mView3dUi.MouseMove(event);
}

void OmView3d::mouseDoubleClickEvent(QMouseEvent * event)
{
	resetBackoff();
	mView3dUi.MouseDoubleClick(event);
}

void OmView3d::mouseWheelEvent(QWheelEvent * event)
{
	resetBackoff();
	mView3dUi.MouseWheel(event);
}

void OmView3d::keyPressEvent(QKeyEvent * event)
{
	resetBackoff();
	mView3dUi.KeyPress(event);
}

void OmView3d::wheelEvent ( QWheelEvent * event )
{
	resetBackoff();
	mouseWheelEvent(event);
}

/////////////////////////////////
///////          Omni Event

/*
 *	Calls function for relevant preferences and redraws,
 *	otherwise event is ignored.
 */
void OmView3d::PreferenceChangeEvent(OmPreferenceEvent * event)
{
	switch (event->GetPreference()) {

	case OM_PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL:
	case OM_PREF_VIEW3D_SHOW_VIEWBOX_BOOL:
	case OM_PREF_VIEW3D_SHOW_INFO_BOOL:
	case OM_PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL:
		UpdateEnabledWidgets();
		break;

	case OM_PREF_VIEW3D_BACKGROUND_COLOR_V3F:
		SetBackgroundColor();
		break;

	case OM_PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL:
		SetBlending();
		break;

	case OM_PREF_VIEW3D_CAMERA_FAR_PLANE_FLT:
	case OM_PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT:
	case OM_PREF_VIEW3D_CAMERA_FOV_FLT:
		//SetCameraPerspective();
		break;

	case OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT:
		break;

	default:
		return;
	}

	updateGL();
}

void OmView3d::SegmentObjectModificationEvent(OmSegmentEvent * event)
{
	resetBackoff();
	updateGL();
}

void OmView3d::VoxelModificationEvent(OmVoxelEvent * event)
{
	resetBackoff();
	updateGL();
}

void OmView3d::SegmentDataModificationEvent(OmSegmentEvent * event)
{
	resetBackoff();
	updateGL();
}

void OmView3d::SystemModeChangeEvent(OmSystemModeEvent * event)
{
	resetBackoff();
	updateGL();
}

void OmView3d::ViewBoxChangeEvent(OmViewEvent * event)
{
	resetBackoff();
	updateGL();
}

void OmView3d::View3dRedrawEvent(OmView3dEvent * event)
{
	resetBackoff();
	updateGL();
}

void OmView3d::View3dRedrawEventFromCache(OmView3dEvent * event)
{
	updateGL();
	increaseBackoff ();
}

void OmView3d::View3dUpdatePreferencesEvent(OmView3dEvent * event)
{
	//UpdateEnabledWidgets();
	//updateGL();
}

/////////////////////////////////
///////          Gl Actions

/*
 *	Returns a vector names of closest picked result for given draw options.
 */
bool OmView3d::PickPoint(Vector2 < int >point2d, vector < unsigned int >&rNamesVec)
{
	//clear name vector
	rNamesVec.clear();

	//setup selection mode
	startPicking(point2d.x, point2d.y, mCamera.GetPerspective().array);

	//render selectable points
	//Draw(drawOps); //OmBitfield drawOps,
	Draw(DRAWOP_LEVEL_ALL | DRAWOP_SEGMENT_FILTER_SELECTED | DRAWOP_RENDERMODE_SELECTION);
	Draw(DRAWOP_LEVEL_ALL | DRAWOP_SEGMENT_FILTER_UNSELECTED | DRAWOP_RENDERMODE_SELECTION);

	//get number of hits
	int hits = stopPicking();

	//if hits < 0, then buffer overflow
	if (hits < 0) {
		printf("OmView3d::PickPoint: hit buffer overflow\n");
		return false;
	}
	//if no hits, success
	if (hits == 0)
		return true;

	//number of names in closest hit
	int numNames;

	//pointer to closest hit names
	int *pNames;
	processHits(hits, &pNames, &numNames);

	//add names from array to names vec
	for (int i = 0; i < numNames; i++) {
		rNamesVec.push_back(pNames[i]);
	}

	//success
	return true;
}

/*
 *	Unproject a 2d point to a 3d point using the depth buffer.
 *	Returns if unproject is valid (not valid if no depth value at pixel).
 */

bool OmView3d::UnprojectPoint(Vector2i point2d, Vector3f & point3d, float z_scale_factor)
{

	//apply camera modelview matrix
	mCamera.ApplyModelview();

	//unproject point2d
	double point3dv[3];
	if (unprojectPixel(point2d.x, point2d.y, point3dv, z_scale_factor) < 0)
		return false;

	//return point3d
	point3d = Vector3f(point3dv[0], point3dv[1], point3dv[2]);
	return true;
}

/////////////////////////////////
///////          Widget Methods

void OmView3d::UpdateEnabledWidgets()
{

	//set widgets enabled
	bool highlight_widget_state = OmPreferences::GetBoolean(OM_PREF_VIEW3D_HIGHLIGHT_ENABLED_BOOL);
	mView3dWidgetManager.SetEnabled(VIEW3D_WIDGET_ID_SELECTION, highlight_widget_state);

	bool viewbox_widget_state = OmPreferences::GetBoolean(OM_PREF_VIEW3D_SHOW_VIEWBOX_BOOL);
	mView3dWidgetManager.SetEnabled(VIEW3D_WIDGET_ID_VIEWBOX, viewbox_widget_state);

	bool info_widget_state = OmPreferences::GetBoolean(OM_PREF_VIEW3D_SHOW_INFO_BOOL);
	mView3dWidgetManager.SetEnabled(VIEW3D_WIDGET_ID_INFO, info_widget_state);

	bool extent_widget = OmPreferences::GetBoolean(OM_PREF_VIEW3D_SHOW_CHUNK_EXTENT_BOOL);
	mView3dWidgetManager.SetEnabled(VIEW3D_WIDGET_ID_CHUNK_EXTENT, extent_widget);

}

/////////////////////////////////
///////          Draw Methods

/*
 *	Root of drawing tree.  
 *	Called from updateGL() and picking calls.
 */
void OmView3d::Draw(OmBitfield cullerOptions)
{

	// clear buffer
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// save the init modelview
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//apply camera modelview matrix
	mCamera.ApplyModelview();

	//if drawing volumes
	if (cullerOptions & DRAWOP_LEVEL_VOLUME) {

		//if in rendering mode
		if (cullerOptions & DRAWOP_RENDERMODE_RENDER) {

			//draw selected and write to stencil (for use with highlighting outline)
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			DrawVolumes(cullerOptions | DRAWOP_SEGMENT_FILTER_SELECTED);
			glDisable(GL_STENCIL_TEST);

			//draw unselected
			//if transparent unselected, disable writing to depth buffer
			if (OmPreferences::GetBoolean(OM_PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL))
				glDepthMask(GL_FALSE);
			//draw unselected segments
			DrawVolumes(cullerOptions | DRAWOP_SEGMENT_FILTER_UNSELECTED |
				    DRAWOP_SEGMENT_COLOR_TRANSPARENT);
			//always renable writing to depth buffer
			glDepthMask(GL_TRUE);
		}
		//if in selection mode
		if (cullerOptions & DRAWOP_RENDERMODE_SELECTION) {
			DrawVolumes(cullerOptions);
		}

		//draw edit selection
		DrawEditSelectionVoxels();
	}

	//debug("FIXME", << "cullerOptions & DRAWOP_DRAW_WIDGETS" << (cullerOptions & DRAWOP_DRAW_WIDGETS) << endl;
	if (cullerOptions & DRAWOP_DRAW_WIDGETS) {
		DrawWidgets();
	}
	//pop to init modelview
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

/*
 *	Draw VolumeManager to using this View3d's camera.
 */
void OmView3d::DrawVolumes(OmBitfield cullerOptions)
{

	//draw focus axis
	mCamera.DrawFocusAxis();

	//setup culler to current projection-modelview matrix
	OmVolumeCuller culler(mCamera.GetProjModelViewMatrix(),
			      mCamera.GetPosition(), mCamera.GetFocus(), cullerOptions);

	//initiate volume manager draw tree
	OmVolume::Draw(culler);
}

void OmView3d::DrawEditSelectionVoxels()
{

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	//offset selection voxels to be on top
	glPolygonOffset(-1.0f, -1.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	//check if there are any voxels to draw
	if (OmSegmentEditor::GetSelectedVoxels().size())
		OmVolume::DrawEditSelectionVoxels();

	glPopAttrib();
}

/////////////////////////////////
///////          Draw Settings

void OmView3d::SetBackgroundColor()
{
	Vector3f bg_color = OmPreferences::GetVector3f(OM_PREF_VIEW3D_BACKGROUND_COLOR_V3F);
	glClearColor(bg_color.r, bg_color.g, bg_color.b, 1);
}

void OmView3d::SetCameraPerspective()
{

	float near = OmPreferences::GetFloat(OM_PREF_VIEW3D_CAMERA_NEAR_PLANE_FLT);
	float far = OmPreferences::GetFloat(OM_PREF_VIEW3D_CAMERA_FAR_PLANE_FLT);
	float fov = OmPreferences::GetFloat(OM_PREF_VIEW3D_CAMERA_FOV_FLT);

	Vector4 < float >perspective(fov, (float)(400) / 300, near, far);

	mCamera.SetPerspective(perspective);
	mCamera.ResetModelview();
}

void OmView3d::SetBlending()
{
	if (OmPreferences::GetBoolean(OM_PREF_VIEW3D_TRANSPARENT_UNSELECTED_BOOL))
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

/*
 *	Draw all enabled widgets.
 */
void OmView3d::DrawWidgets()
{
	mView3dWidgetManager.CallEnabled(&OmView3dWidget::Draw);
}

/////////////////////////////////
///////
///////         Utility Functions
///////

/*
 * Initialize lights
 * http://www.songho.ca/opengl/gl_vbo.html
 */
void initLights()
{
	// set up light colors (ambient, diffuse, specular)
	GLfloat lightKa[] = { .2f, .2f, .2f, 1.0f };	// ambient light
	GLfloat lightKd[] = { .7f, .7f, .7f, 1.0f };	// diffuse light
	GLfloat lightKs[] = { 1, 1, 1, 1 };	// specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);

	glMateriali(GL_FRONT, GL_SHININESS, 96);

	// position the light
	float lightPos[4] = { 0, 0, 1000, 1 };	// positional light
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);	// enable light source after configuration
}

/*
 int OmView3d::faceAtPosition(const QPoint&pos) 
 { 
 //debug("FIXME", << "face at" << endl;
 
 const int MaxSize = 512; 
 GLuint buffer[MaxSize]; 
 GLint viewport[4]; 
 glGetIntegerv(GL_VIEWPORT, viewport); 
 glSelectBuffer(MaxSize, buffer); 
 glRenderMode(GL_SELECT); 
 glInitNames(); 
 glPushName(0); 
 glMatrixMode(GL_PROJECTION); 
 glPushMatrix(); 
 glLoadIdentity(); 
 gluPickMatrix(GLdouble(pos.x()), GLdouble(viewport[3] - pos.y()), 
 5.0, 5.0, viewport); 
 GLfloat x = GLfloat(width())/height(); 
 glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0); 
 draw(); 
 glMatrixMode(GL_PROJECTION); 
 glPopMatrix(); 
 if (!glRenderMode(GL_RENDER)) 
 return -1; 
 return buffer[3]; 
 } 
 */
