//
// TODO: GIVE CREDIT TO Eugene Hsu, this is mostly HIS CODE
//       and based on 6.839 demo code for camera!
//

#ifndef OM_GL_CAMERA_H
#define OM_GL_CAMERA_H

/*
 *
 */

#include "omCameraMovement.h"

#include "common/omStd.h"
#include "common/omException.h"

class OmCameraArcBall;

enum CameraMovementType {
  CAMERA_NONE,
  CAMERA_PAN,
  CAMERA_ZOOM,
  CAMERA_ORBIT_ROTATE,
  CAMERA_LOOKAT_ROTATE
};

class OmCamera {

 public:
  OmCamera();

  //state
  Vector4<int> GetViewport() const;
  void SetViewport(const Vector4<int> &viewport);
  void ApplyViewport();

  Vector4<float> GetPerspective() const;
  void SetPerspective(const Vector4<int> &viewport);
  void ApplyPerspective();

  void ApplyReshape(const Vector2<int> &dims);

  void ResetModelview();
  void UpdateModelview();
  void ApplyModelview();

  //accessors
  Vector3f GetPosition();

  Vector3f GetFocus();
  void SetFocus(const Vector3f &focus);

  float GetDistance();
  void SetDistance(float distance);

  Quaternionf GetOrbitRotation();
  void SetOrbitRotation(const Quaternionf &);

  Quaternionf GetLookAtRotation();
  void SetLookAtRotation(const Quaternionf &);

  const Matrix4<float> &GetModelViewMatrix() const;
  const Matrix4<float> &GetProjectionMatrix() const;
  Matrix4<float> GetProjModelViewMatrix() const;

  //draw
  void DrawFocusAxis();

  //movement
  void MovementStart(CameraMovementType type, const Vector2<float> &point);
  void MovementUpdate(const Vector2<float> &point);
  void MovementEnd(const Vector2<float> &point);

 private:
  //viewport props
  Vector4<int> mViewport;  // lower left x, lower left y, width, height
  Vector4<float>
      mPerspective;  // field of view, aspect ratio, near clip, far clip

  //defines camera position
  float mDistance;
  Quaternion<float> mOrbitRotation;
  Quaternion<float> mLookAtRotation;

  //movement matricies
  Matrix4<float> mLookAtMatrix;
  Matrix4<float> mZoomMatrix;
  Matrix4<float> mOrbitMatrix;
  Matrix4<float> mFocusMatrix;

  //derived matricies
  Matrix4<float> mModelViewMatrix;
  Matrix4<float> mProjectionMatrix;

  // movement members
  CameraMovementType mMovementType;

  OmCameraZoom mZoomer;
  OmCameraPan mPanner;
  OmCameraArcBall mArcBall;

  friend class OmCameraArcBall;
  friend class OmCameraPan;
  friend class OmCameraZoom;
};

#endif
