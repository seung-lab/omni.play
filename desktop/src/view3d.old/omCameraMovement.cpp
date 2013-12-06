#include "omCameraMovement.h"
#include "omCamera.h"
#include "common/logging.h"

static const float EPSILON = 1.0e-5;

/////////////////////////////////////
//////////
//////////      CameraArcBall Methods
//////////

void OmCameraArcBall::Click(const Vector2<float>& point) {

  switch (mpCamera->mMovementType) {
    case CAMERA_LOOKAT_ROTATE:
      mStartRotation = mpCamera->mLookAtRotation;
      break;
    case CAMERA_ORBIT_ROTATE:
      mStartRotation = mpCamera->mOrbitRotation;
      break;
    default:
      assert(false);
  }

  MapPointToSphere(point, mStartVector);
}

void OmCameraArcBall::Drag(const Vector2<float>& point) {
  // find end vector
  MapPointToSphere(point, mEndVector);

  // get perpendicular to start and end vectors
  Vector3<float> perp_vector;
  perp_vector.cross(mStartVector, mEndVector);

  // compute length of perpendicular
  if (perp_vector.length() > EPSILON) {  // if non-zero

    // update specific matrix depending on what type of rotation
    Quaternion<float> rotation;

    switch (mpCamera->mMovementType) {
      case CAMERA_LOOKAT_ROTATE:
        rotation = mpCamera->mLookAtRotation;
        break;

      case CAMERA_ORBIT_ROTATE:
        rotation = mpCamera->mOrbitRotation;
        break;

      default:
        assert(false);
    }

    // apply new rotation to old rotation
    rotation = Quaternion<float>(Vector3<float>::dot(mStartVector, mEndVector),
                                 perp_vector) *
               mStartRotation;
    rotation.normalize();

    switch (mpCamera->mMovementType) {
      case CAMERA_LOOKAT_ROTATE:
        mpCamera->SetLookAtRotation(rotation);
        break;

      case CAMERA_ORBIT_ROTATE:
        mpCamera->SetOrbitRotation(rotation);
        break;

      default:
        assert(false);
    }
  }
}

/*
 *  Returns a vector on a sphere mapped from a given point in the viewport.
 */
void OmCameraArcBall::MapPointToSphere(const Vector2<float>& point,
                                       Vector3<float>& vector) {
  // get viewport and scale
  const Vector4<int>& r_viewport = mpCamera->mViewport;
  // assert((viewport.width > 1) && (viewport.height > 1));
  if ((r_viewport.width < 1) || (r_viewport.height < 1)) return;

  // get adjusted dimensions of viewport
  mAdjustWidth = 1.0f / ((r_viewport.width - 1) * 0.5f);
  mAdjustHeight = 1.0f / ((r_viewport.height - 1) * 0.5f);

  // scale point from [0, dim) to [-1, 1]
  Vector2<float> scaled_point = Vector2<float>(point.x * mAdjustWidth - 1.0f,
                                               1.0f - point.y * mAdjustHeight);

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

void OmCameraPan::Click(const Vector2<float>& point) {
  mStartCenter = mpCamera->GetFocus();
  mStartPoint = point;
}

void OmCameraPan::Drag(const Vector2<float>& point) {
  Vector4<int>& r_viewport = mpCamera->mViewport;

  // start
  double sx = mStartPoint[0] - r_viewport[0];
  double sy = mStartPoint[1] - r_viewport[1];

  // current
  double cx = point.x - r_viewport[0];
  double cy = point.y - r_viewport[1];

  // compute "distance" of image plane (wrt projection matrix)
  Vector4<float>& r_perspective = mpCamera->mPerspective;
  double d =
      double(r_viewport[3]) / 2.0 / tan(r_perspective[0] * M_PI / 180.0 / 2.0);

  // compute up plane intersect of clickpoint (wrt fovy)
  double su = -sy + r_viewport[3] / 2.0;
  double cu = -cy + r_viewport[3] / 2.0;

  // compute right plane intersect of clickpoint (ASSUMED FOVY is 1)
  double sr = (sx - r_viewport[2] / 2.0);
  double cr = (cx - r_viewport[2] / 2.0);

  // this maps move
  float distance = mpCamera->GetDistance();
  Vector2<float> move(cr - sr, cu - su);
  move *= -distance / d;

  // update center
  Vector3<float> center = mpCamera->GetFocus();
  Matrix3<float> orbit_mat = mpCamera->mOrbitRotation.getRotationMatrix();

  // Matrix3<float> rotation_mat = orbit_mat * lookat_mat;

  // update center
  center = mStartCenter + orbit_mat.getRow(0) * move.x +
           orbit_mat.getRow(1) * move.y;

  // update pan matrix
  mpCamera->SetFocus(center);
}

/////////////////////////////////////
//////////
//////////      CameraZoom Methods
//////////

void OmCameraZoom::Click(const Vector2<float>& point) {
  mOldViewMatrix = mpCamera->GetModelViewMatrix();
  mStartDistance = mpCamera->GetDistance();
  mStartPoint = point;
}

void OmCameraZoom::Drag(const Vector2<float>& point) {
  const Vector4<int>& r_viewport = mpCamera->mViewport;
  int sy = mStartPoint[1] - r_viewport[1];
  int cy = point.y - r_viewport[1];

  double delta = double(cy - sy) / r_viewport[3];

  // exponential zoom factor
  mpCamera->SetDistance(std::max(mStartDistance, 1.0f) * exp(delta));
}
