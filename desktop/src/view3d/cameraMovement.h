#pragma once
#include "precomp.h"

#include "common/common.h"

namespace om {
namespace v3d {

class Camera;

class CameraMovement {
 public:
  CameraMovement(Camera& c) : camera_(c) {}

 protected:
  Camera& camera_;
  Matrix4f oldViewMatrix_;
};

/*
 * Quaternion ArcBall for camera movement.
 * Based off of http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=48
 */
class CameraArcBall : public CameraMovement {
 public:
  CameraArcBall(Camera& camera_) : CameraMovement(camera_) {}

  void Click(const Vector2<float>& point);
  void Drag(const Vector2<float>& point);

 private:
  void MapPointToSphere(const Vector2<float>& point, Vector3<float>& vector);

  float adjustWidth_, adjustHeight_;
  Vector3<float> startVector_, endVector_;  // click and drag vector

  Quaternion<float> startRotation_;
};

class CameraPan : public CameraMovement {
 public:
  CameraPan(Camera& camera_) : CameraMovement(camera_) {}

  void Click(const Vector2<float>& point);
  void Drag(const Vector2<float>& point);

 private:
  Vector2<float> startPoint_;
  Vector3<float> startCenter_;
};

class CameraZoom : public CameraMovement {
 public:
  CameraZoom(Camera& camera_) : CameraMovement(camera_) {}

  void Click(const Vector2<float>& point);
  void Drag(const Vector2<float>& point);

 private:
  float startDistance_;
  Vector2<float> startPoint_;
};
}
}  // om::v3d::
