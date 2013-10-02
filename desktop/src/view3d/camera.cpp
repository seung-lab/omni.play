#include "camera.h"
#include "view3d/gl.h"

namespace om {
namespace v3d {

static const float INITIAL_DISTANCE = 20;
static const Vector3f INITIAL_DIRECTION(1, 0, 0);  // direction
static const Vector3f INITIAL_FOCUS(0, 0, 0);      // focus
static const Vector3f INITIAL_UP(0, 1, 0);         // orientation

Camera::Camera() : zoomer_(*this), panner_(*this), arcBall_(*this) {

  // initialize view matrix
  modelViewMatrix_ = Matrix4<float>::IDENTITY;
  lookAtMatrix_ = Matrix4<float>::IDENTITY;
  zoomMatrix_ = Matrix4<float>::IDENTITY;
  orbitMatrix_ = Matrix4<float>::IDENTITY;
  focusMatrix_ = Matrix4<float>::IDENTITY;
}

/////////////////////////////////
///////          State Accessors

/*
 * Specifies the affine transformation of x and y from
 * normalized device coordinates to	window coordinates
 */
Vector4<int> Camera::GetViewport() const { return viewport_; }

void Camera::SetViewport(const Vector4<int>& view) {
  viewport_ = view;
  ApplyViewport();
}

void Camera::ApplyViewport() {
  glViewport(viewport_.lowerLeftX, viewport_.lowerLeftY, viewport_.width,
             viewport_.height);
}

/*
 *	Set the camera perspective
 */
Vector4<float> Camera::GetPerspective() const { return perspective_; }

void Camera::SetPerspective(const Vector4<int>& persp) {
  perspective_ = persp;
  ApplyPerspective();
}

/*
 * Apply camera perspective to GL state.
 */
void Camera::ApplyPerspective() {
  // setup transform from eye space coordinates into clip coordinates
  // http://www.opengl.org/resources/faq/technical/viewing.htm

  // perspective changes should be the only transforms applied to the projection
  // matrix
  glMatrixMode(GL_PROJECTION);

  // refresh projection matrix
  glLoadIdentity();
  gluPerspective(perspective_[0], perspective_[1], perspective_[2], 10000000);

  // store projection matrix
  glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix_.ml);

  // back to modelview matrix for geometry
  glMatrixMode(GL_MODELVIEW);
}

/*
 * Apply window dimension change.
 */
void Camera::ApplyReshape(const Vector2<int>& dims) {
  // update viewport
  viewport_.width = dims.x;
  viewport_.height = dims.y;

  // update aspect ratio in perspective
  perspective_[1] = float(dims.x) / dims.y;

  // apply changes;
  ApplyViewport();
  ApplyPerspective();
}

/*
 * Resets to initial modelview matrix.  First GL call to be made.
 */

void Camera::ResetModelview() {
  /// use gluLookAt to compute initial orbit
  // set to modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // compute rotation matrix as if at zero looking at initial direction
  gluLookAt(0, 0, 0, INITIAL_DIRECTION.x, INITIAL_DIRECTION.y,
            INITIAL_DIRECTION.z, INITIAL_UP.x, INITIAL_UP.y, INITIAL_UP.z);

  // set orbit rotation from lookat
  glGetFloatv(GL_MODELVIEW_MATRIX, orbitMatrix_.ml);

  // extract rotation
  Matrix3f orbit_rotation_matrix;
  orbitMatrix_.get3x3SubMatrix(orbit_rotation_matrix);
  SetOrbitRotation(Quaternionf(orbit_rotation_matrix));

  // set lookat to be head on
  SetLookAtRotation(Quaternion<float>::IDENTITY);

  // set initial focus and distance
  SetFocus(INITIAL_FOCUS);
  SetDistance(INITIAL_DISTANCE);

  // update with reset values
  UpdateModelview();
}

void Camera::UpdateModelview() {
  modelViewMatrix_ = lookAtMatrix_ * zoomMatrix_ * orbitMatrix_ * focusMatrix_;
}

/*
 * Apply ModelView to GL (every draw call).
 */
void Camera::ApplyModelview() {

  // load model view matrix into GL
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(modelViewMatrix_.ml);
}

/////////////////////////////////
///////
///////          Property Accessors
///////

Vector3f Camera::GetPosition() {
  bool valid;
  return modelViewMatrix_.getInverse(valid).getTranslation();
}

Vector3f Camera::GetFocus() { return -focusMatrix_.getTranslation(); }

void Camera::SetFocus(const Vector3f& focus) {
  focusMatrix_.setTranslation(-focus);
  UpdateModelview();
}

float Camera::GetDistance() { return -zoomMatrix_.ml[14]; }

void Camera::SetDistance(float distance) {
  // z-component of translation vector
  zoomMatrix_.ml[14] = -distance;
  UpdateModelview();
}

Quaternionf Camera::GetOrbitRotation() { return orbitRotation_; }

void Camera::SetOrbitRotation(const Quaternionf& rRotation) {
  orbitRotation_ = rRotation;
  orbitMatrix_.set3x3SubMatrix(orbitRotation_.getRotationMatrix(), 0, 0);
  UpdateModelview();
}

Quaternionf Camera::GetLookAtRotation() { return lookAtRotation_; }

void Camera::SetLookAtRotation(const Quaternionf& rRotation) {
  lookAtRotation_ = rRotation;
  lookAtMatrix_.set3x3SubMatrix(lookAtRotation_.getRotationMatrix(), 0, 0);
  UpdateModelview();
}

const Matrix4<float>& Camera::GetModelViewMatrix() const {
  return modelViewMatrix_;
}

const Matrix4<float>& Camera::GetProjectionMatrix() const {
  return projectionMatrix_;
}

Matrix4<float> Camera::GetProjModelViewMatrix() const {
  return projectionMatrix_ * modelViewMatrix_;
}

/////////////////////////////////
///////          Draw

void Camera::DrawFocusAxis() {
  // store
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // draw at focus point
  glLoadIdentity();
  glLoadMatrixf((lookAtMatrix_ * zoomMatrix_ * orbitMatrix_).ml);

  // scale to maintain axis size
  const float scale = GetDistance() / 30.0f;
  glScalef(scale, scale, scale);

  // draw axis
  om::gl::glDrawAxis(2);

  // restore original modelview
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

/////////////////////////////////
///////          Movement

void Camera::MovementStart(CameraMovementType type,
                           const Vector2<float>& point) {
  movementType_ = type;

  switch (type) {
    case CameraMovementType::PAN:
      panner_.Click(point);
      break;
    case CameraMovementType::ZOOM:
      zoomer_.Click(point);
      break;
    case CameraMovementType::LOOKAT_ROTATE:
      arcBall_.Click(point);
      break;
    case CameraMovementType::ORBIT_ROTATE:
      arcBall_.Click(point);
      break;
    case CameraMovementType::NONE:
      break;
  }
}

void Camera::MovementUpdate(const Vector2<float>& point) {
  switch (movementType_) {
    case CameraMovementType::PAN:
      panner_.Drag(point);
      break;
    case CameraMovementType::ZOOM:
      zoomer_.Drag(point);
      break;
    case CameraMovementType::LOOKAT_ROTATE:
      arcBall_.Drag(point);
      break;
    case CameraMovementType::ORBIT_ROTATE:
      arcBall_.Drag(point);
      break;
    case CameraMovementType::NONE:
      break;
  }
}

void Camera::MovementEnd(const Vector2<float>&) {
  movementType_ = CameraMovementType::NONE;
}
}
}  // om::v3d::
