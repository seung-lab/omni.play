#include "view3d/omniEventListener.h"
#include "mesh/omVolumeCuller.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentUtils.hpp"
#include "system/omAppState.hpp"
#include "system/omLocalPreferences.hpp"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "view3d/camera.h"
#include "view3d/drawer.hpp"
#include "view3d/gl.h"
#include "view3d/omniEventListener.h"
#include "view3d/ui.h"
#include "view3d/view3d.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "widgets/widgets.hpp"
#include "gui/controls/viewControls.hpp"

DECLARE_ZiARG_bool(noView3dThrottle);

namespace om {
namespace v3d {

/*
 *  Constructs View3d widget that shares with the primary widget.
 */
View3d::View3d(QWidget* parent, OmViewGroupState& vgs)
#ifdef ZI_OS_MACOS
    : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent,
                vgs->get3dContext())
#else
    : QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer), parent)
#endif
      ,
      prefs_(new om::prefs::View3d()),
      widgets_(new Widgets(*this, vgs)),
      ui_(new Ui(*this, vgs)),
      drawStatus_(new DrawStatus()),
      camera_(new Camera()),
      omniEventListener_(new OmniEventListener(*this, vgs)),
      segmentations_(SegmentationDataWrapper::GetPtrVec()),
      vgs_(vgs), viewControls_(new ViewControls(this, &vgs)) {

  const auto& primary_coords = vgs.Segmentation().GetSegmentation()->Coords();

  drawer_.reset(new Drawer(*widgets_, *drawStatus_, *camera_,
                           segmentations_, vgs, primary_coords));

  // set keyboard policy
  setFocusPolicy(Qt::ClickFocus);
  setAttribute(Qt::WA_AcceptTouchEvents);

  UpdatePreferences();

  drawTimer_.stop();
  connect(&drawTimer_, SIGNAL(timeout()), this, SLOT(updateGL()));

  elapsed_.start();

  grabGesture(Qt::PanGesture);
  grabGesture(Qt::PinchGesture);
  grabGesture(Qt::SwipeGesture);
}

View3d::~View3d() {
  if (drawTimer_.isActive()) {
    drawTimer_.stop();
  }
}

/////////////////////////////////
///////          Accessor Methods

Camera& View3d::GetCamera() { return *camera_; }

/////////////////////////////////
///////          GL Event Methods

void View3d::initializeGL() {
  // The initializeGL() function is called just once, before paintGL() is
  // called.

  glShadeModel(GL_SMOOTH);  // shading mathod: GL_SMOOTH or GL_FLAT
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  // 4-byte pixel alignment

  // enable /disable features
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);  // normalize normals for lighting

  // set material properties
  glEnable(GL_COLOR_MATERIAL);  // cause material to track current color
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);  // cause ambient and
                                                      // diffust to track color

  float black[4] = {0, 0, 0, 0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, black);
  glMaterialfv(GL_FRONT, GL_SPECULAR, black);

  // set drawing properties
  setBackgroundColor();    // background color
  glClearStencil(0);       // clear stencil buffer
  glClearDepth(1.0f);      // 0 is mynear, 1 is myfar
  glDepthFunc(GL_LEQUAL);  // drawn if depth value is less than or equal
                           // than previous stored depth value

  glDisable(GL_BLEND);
  glEnable(GL_BLEND);  // enable blending for transparency
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  initLights();

  Vector4i viewport(0, 0, 400, 300);
  camera_->SetViewport(viewport);  // set viewport

  setCameraPerspective();  // camera props
}

/*
 *  Window resize event
 */
void View3d::resizeGL(int width, int height) {
  camera_->ApplyReshape(Vector2i(width, height));
}

/*
 *  Paint window event.
 */
void View3d::paintGL() {
  elapsed_.restart();
  drawer_->PrimaryDraw();
}

/*
 * Interface to the real updateGL.
 */
void View3d::TimedUpdate() {
  if (ZiARG_noView3dThrottle) {
    updateGL();
    return;
  }

  if (elapsed_.elapsed() > 1000) {
    elapsed_.restart();
    updateGL();
  }

  if (drawTimer_.isActive()) {
    drawTimer_.stop();
    drawTimer_.start(100);
    drawTimer_.setSingleShot(true);
  } else {
    drawTimer_.start(100);
    drawTimer_.setSingleShot(true);
  }
}

/////////////////////////////////
///////          QEvent Methods

void View3d::mousePressEvent(QMouseEvent* event) {
  try {
    if (viewControls_->mousePressEvent(event)) {
      return;
    }
    ui_->MousePressed(event);
  }
  catch (...) {
  }
}

void View3d::mouseReleaseEvent(QMouseEvent* event) {
  if (viewControls_->mouseReleaseEvent(event)) {
    return;
  }
  ui_->MouseRelease(event);
}


void View3d::mouseMoveEvent(QMouseEvent* event) {
  if (!GetCamera().IsMoving()) {
    viewControls_->mouseMoveEvent(event);
    return;
  }
  ui_->MouseMove(event);
}

void View3d::mouseDoubleClickEvent(QMouseEvent* event) {
  if (viewControls_->mouseDoubleClickEvent(event)) {
    return;
  }
  ui_->MouseDoubleClick(event);
}

void View3d::keyPressEvent(QKeyEvent* event) {
  if (viewControls_->keyPressEvent(event)) {
    return;
  }
  ui_->KeyPress(event);
}

void View3d::keyReleaseEvent(QKeyEvent* event) {
  if (viewControls_->keyReleaseEvent(event)) {
    return;
  }
  ui_->KeyRelease(event);
}

void View3d::wheelEvent(QWheelEvent* event) {
  if (viewControls_->wheelEvent(event)) {
    return;
  }
  ui_->MouseWheel(event);
}

/////////////////////////////////
///////          Gl Actions

/*
 *  Returns a vector names of closest picked result for given draw options.
 *
 * causes localized redraw (but all depth info stored in selection buffer)
 *
 */
bool View3d::pickPoint(const Vector2i& vec, std::vector<uint32_t>& rNamesVec) {
  updateGL();

  // clear name vector
  rNamesVec.clear();

  // setup selection mode
  om::gl::startPicking(vec.x, vec.y, camera_->GetPerspective().array);

  // render selectable points
  drawer_->DrawPickPoint();

  // get number of hits
  int hits = om::gl::stopPicking();

  // if hits < 0, then buffer overflow
  if (hits < 0) {

    log_errors << "View3d::PickPoint: hit buffer overflow: " << hits;
    return false;
  }

  // if no hits, success
  if (hits == 0) {
    return true;
  }

  // number of names in closest hit
  int numNames;

  // pointer to closest hit names
  int* pNames;
  om::gl::processHits(hits, &pNames, &numNames);

  // add names from array to names vec
  for (auto i = 0; i < numNames; i++) {
    rNamesVec.push_back(pNames[i]);
  }

  // success
  return true;
}

boost::optional<SegmentDataWrapper> View3d::FindSegment(int x, int y) {
  const Vector2i vec(x, y);

  std::vector<uint32_t> result;

  const bool valid_pick = pickPoint(vec, result);

  boost::optional<SegmentDataWrapper> segmentDataWrapper;

  // if valid and return count
  if (!valid_pick || (result.size() < 2)) {
    return segmentDataWrapper;
  }

  // ensure valid segmentID
  segmentDataWrapper = SegmentDataWrapper(result[0], result[1]);

  return segmentDataWrapper;
}

/*
 *  Convert screen coordinates into 3d global coordinates by
 *  unprojecting the 2d point to a 3d point using the depth buffer.
 *  Returns 3d global coords if unproject is valid (empty optional if 
 *  not valid when no depth value at pixel).
 */
boost::optional<om::coords::Global> View3d::FindGlobalCoords(int x, int y) {
  const Vector2i vec(x, y);

  boost::optional<om::coords::Global> globalCoords;

  // apply camera modelview matrix
  camera_->ApplyModelview();

  // unproject Vector2i
  double point3dv[3];
  if (om::gl::unprojectPixel(x, y, point3dv) < 0)
    return globalCoords;

  // return point3d
  globalCoords = om::coords::Global{point3dv[0], point3dv[1], point3dv[2]};
  return globalCoords;
}

/////////////////////////////////
///////          Widget Methods

void View3d::updateEnabledWidgets() { widgets_->UpdateEnabledWidgets(); }

/////////////////////////////////
///////          Draw Methods

/*
 *  Draw VolumeManager to using this View3d's camera.
 */
void View3d::DrawChunkBoundaries() { drawer_->DrawChunkBoundaries(); }

/*
 *  Draw segmentation boundaries
 */
void View3d::DrawSegmentationBoundaries() { drawer_->DrawSegmentationBoundaries(segmentations_); }

/////////////////////////////////
///////          Draw Settings

void View3d::UpdatePreferences() {
  *prefs_ = OmPreferences::V3dPrefs();
  updateEnabledWidgets();
  setBackgroundColor();
  setCameraPerspective();
}

void View3d::setBackgroundColor() {
  Vector3f bg_color = prefs_->BACKGROUND_COLOR;
  glClearColor(bg_color.r, bg_color.g, bg_color.b, 1);
}

void View3d::setCameraPerspective() {
  float mynear = prefs_->CAMERA_NEAR_PLANE;
  float fov = prefs_->CAMERA_FOV;
  static const float myfar = 10000000.0;
  Vector4<float> perspective(fov, (float)(400) / 300, mynear, myfar);

  camera_->SetPerspective(perspective);
  camera_->ResetModelview();
}

/////////////////////////////////
///////
///////         Utility Functions
///////

/*
 * Initialize lights
 * http://www.songho.ca/opengl/gl_vbo.html
 */
void View3d::initLights() {
  // set up light colors (ambient, diffuse, specular)
  GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};  // ambient light
  GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
  GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

  float specReflection[] = {0.8f, 0.8f, 0.8f, 1.0f};
  glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);

  glMateriali(GL_FRONT, GL_SHININESS, 96);

  // position the light
  float lightPos[4] = {0, 0, 1000, 1};  // positional light
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

  glEnable(GL_LIGHT0);  // enable light source after configuration
}

QSize View3d::sizeHint() const {
  const QSize s = OmAppState::GetViewBoxSizeHint();

  // TODO: offset is only 76 if tabs are present in the upper-right dock
  // widget...
  const int offset = 76;

  return QSize(s.width(), s.height() - offset);
}

bool View3d::event(QEvent* e) {
  if (e->type() == QEvent::Gesture) {
    return ui_->GestureEvent(static_cast<QGestureEvent*>(e));
  }
  return QGLWidget::event(e);
}

void View3d::DoZoom(const int direction) {
  const float dist = camera_->GetDistance();
  camera_->SetDistance(dist - (0.3 * dist * direction));
  updateGL();
}

DrawStatus View3d::Status() { return *drawStatus_; }
}
}  // om::v3d::
