#include "cameraMovement.h"
#include "camera.h"

namespace om {
namespace v3d {

static const float EPSILON = 1.0e-5;

/////////////////////////////////////
//////////
//////////      CameraArcBall Methods
//////////

void CameraArcBall::Click(const Vector2<float>& point) {
  switch (camera_.movementType()) {
    case CameraMovementType::LOOKAT_ROTATE:
      startRotation_ = camera_.lookAtRotation();
      break;
    case CameraMovementType::ORBIT_ROTATE:
      startRotation_ = camera_.orbitRotation();
      break;
    default:
      assert(false);
  }

  MapPointToSphere(point, startVector_);
}

void CameraArcBall::Drag(const Vector2<float>& point) {
  // find end vector
  MapPointToSphere(point, endVector_);

  // get perpendicular to start and end vectors
  Vector3<float> perp_vector;
  perp_vector.cross(startVector_, endVector_);

  // compute length of perpendicular
  if (perp_vector.length() > EPSILON) {  // if non-zero

    // update specific matrix depending on what type of rotation
    Quaternion<float> rotation;

    switch (camera_.movementType()) {
      case CameraMovementType::LOOKAT_ROTATE:
        rotation = camera_.lookAtRotation();
        break;

      case CameraMovementType::ORBIT_ROTATE:
        rotation = camera_.orbitRotation();
        break;

      default:
        assert(false);
    }

    // apply new rotation to old rotation
    rotation = Quaternion<float>(Vector3<float>::dot(startVector_, endVector_),
                                 perp_vector) *
               startRotation_;
    rotation.normalize();

    switch (camera_.movementType()) {
      case CameraMovementType::LOOKAT_ROTATE:
        camera_.SetLookAtRotation(rotation);
        break;

      case CameraMovementType::ORBIT_ROTATE:
        camera_.SetOrbitRotation(rotation);
        break;

      default:
        assert(false);
    }
  }
}

/*
 * Returns a vector on a sphere mapped from a given point in the viewport.
 */
void CameraArcBall::MapPointToSphere(const Vector2<float>& point,
                                     Vector3<float>& vector) {
  // get viewport and scale
  const Vector4<int>& vp = camera_.viewport();

  // assert((viewport.width > 1) && (viewport.height > 1));
  if ((vp.width < 1) || (vp.height < 1)) {
    return;
  }

  // get adjusted dimensions of viewport
  adjustWidth_ = 1.0f / ((vp.width - 1) * 0.5f);
  adjustHeight_ = 1.0f / ((vp.height - 1) * 0.5f);

  // scale point from [0, dim) to [-1, 1]
  Vector2<float> scaled_point(point.x * adjustWidth_ - 1.0f,
                              1.0f - point.y * adjustHeight_);

  // get square length
  float sqr_length = scaled_point.lengthSquared();

  // if point outside of arcball sphere
  if (sqr_length > 1.0f) {
    // return normalized point on sphere
    float norm = 1.0f / sqrtf(sqr_length);
    vector.x = scaled_point.x * norm;
    vector.y = scaled_point.y * norm;
    vector.z = 0;

  } else {
    // return vector to point mapped inside the sphere
    vector.x = scaled_point.x;
    vector.y = scaled_point.y;
    vector.z = sqrtf(1.0f - sqr_length);
  }
}

/////////////////////////////////////
//////////
//////////      CameraPan Methods
//////////

void CameraPan::Click(const Vector2<float>& point) {
  startCenter_ = camera_.GetFocus();
  startPoint_ = point;
}

void CameraPan::Drag(const Vector2<float>& point) {
  Vector4<int>& vp = camera_.viewport();

  // start
  double sx = startPoint_[0] - vp[0];
  double sy = startPoint_[1] - vp[1];

  // current
  double cx = point.x - vp[0];
  double cy = point.y - vp[1];

  // compute "distance" of image plane (wrt projection matrix)
  Vector4<float>& perspective = camera_.perspective();
  double d = double(vp[3]) / 2.0 / tan(perspective[0] * M_PI / 180.0 / 2.0);

  // compute up plane intersect of clickpoint (wrt fovy)
  double su = -sy + vp[3] / 2.0;
  double cu = -cy + vp[3] / 2.0;

  // compute right plane intersect of clickpoint (ASSUMED FOVY is 1)
  double sr = (sx - vp[2] / 2.0);
  double cr = (cx - vp[2] / 2.0);

  // this maps move
  float distance = camera_.GetDistance();
  Vector2<float> move(cr - sr, cu - su);
  move *= -distance / d;

  // update center
  Vector3<float> center = camera_.GetFocus();
  Matrix3<float> orbit_mat = camera_.orbitRotation().getRotationMatrix();

  // update center
  center = startCenter_ + orbit_mat.getRow(0) * move.x +
           orbit_mat.getRow(1) * move.y;

  // update pan matrix
  camera_.SetFocus(center);
}

/////////////////////////////////////
//////////
//////////      CameraZoom Methods
//////////

void CameraZoom::Click(const Vector2<float>& point) {
  oldViewMatrix_ = camera_.GetModelViewMatrix();
  startDistance_ = camera_.GetDistance();
  startPoint_ = point;
}

void CameraZoom::Drag(const Vector2<float>& point) {
  const Vector4<int>& vp = camera_.viewport();
  int sy = startPoint_[1] - vp[1];
  int cy = point.y - vp[1];

  double delta = double(cy - sy) / vp[3];

  // exponential zoom factor
  camera_.SetDistance(std::max(startDistance_, 1.0f) * exp(delta));
}
}
}  // om::v3d::
