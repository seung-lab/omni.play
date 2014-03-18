#pragma once
#include "precomp.h"
//
// TODO: GIVE CREDIT TO Eugene Hsu, this is mostly HIS CODE
//       and based on 6.839 demo code for camera!
//

#include "common/macro.hpp"
#include "cameraMovement.h"
#include "common/exception.h"

namespace om {
namespace v3d {

class CameraArcBall;

enum class CameraMovementType {
  NONE,
  PAN,
  ZOOM,
  ORBIT_ROTATE,
  LOOKAT_ROTATE
};

class Camera {

 public:
  Camera();

  // state
  Vector4<int> GetViewport() const;
  void SetViewport(const Vector4<int>& viewport);
  void ApplyViewport();

  Vector4<float> GetPerspective() const;
  void SetPerspective(const Vector4<int>& viewport);
  void ApplyPerspective();

  void ApplyReshape(const Vector2<int>& dims);

  void ResetModelview();
  void UpdateModelview();
  void ApplyModelview();

  // accessors
  Vector3f GetPosition();

  Vector3f GetFocus();
  void SetFocus(const Vector3f& focus);

  float GetDistance();
  void SetDistance(float distance);

  Quaternionf GetOrbitRotation();
  void SetOrbitRotation(const Quaternionf&);

  Quaternionf GetLookAtRotation();
  void SetLookAtRotation(const Quaternionf&);

  const Matrix4<float>& GetModelViewMatrix() const;
  const Matrix4<float>& GetProjectionMatrix() const;
  Matrix4<float> GetProjModelViewMatrix() const;

  // draw
  void DrawFocusAxis();

  // movement
  void MovementStart(CameraMovementType type, const Vector2<float>& point);
  void MovementUpdate(const Vector2<float>& point);
  void MovementEnd(const Vector2<float>& point);

 private:
  // viewport props
  PROP_REF(Vector4<int>, viewport);       // lower left x, lower left y, width,
                                          // height
  PROP_REF(Vector4<float>, perspective);  // field of view, aspect ratio, near
                                          // clip, far clip

  // defines camera position
  PROP_REF(float, distance);
  PROP_REF(Quaternion<float>, orbitRotation);
  PROP_REF(Quaternion<float>, lookAtRotation);

  // movement matricies
  PROP_REF(Matrix4<float>, lookAtMatrix);
  PROP_REF(Matrix4<float>, zoomMatrix);
  PROP_REF(Matrix4<float>, orbitMatrix);
  PROP_REF(Matrix4<float>, focusMatrix);

  // derived matricies
  PROP_REF(Matrix4<float>, modelViewMatrix);
  PROP_REF(Matrix4<float>, projectionMatrix);

  // movement members
  PROP_REF(CameraMovementType, movementType);

  PROP_REF(CameraZoom, zoomer);
  PROP_REF(CameraPan, panner);
  PROP_REF(CameraArcBall, arcBall);
};
}
}  // om::v3d::
