#include "omVolumeCuller.h"
#include "coordinates/coordinates.h"

#include <QtGlobal>
namespace om {
namespace v3d {
VolumeCuller::VolumeCuller(const Matrix4f& projmodelview,
                           const om::coords::Norm& pos,
                           const om::coords::Norm& focus)
    : mProjModelView(projmodelview), mPosition(pos), mFocus(focus) {
  mFrustumCuller.setup(mProjModelView);
}

const om::coords::Norm& VolumeCuller::GetPosition() const { return mPosition; }

std::shared_ptr<VolumeCuller> VolumeCuller::GetTransformedCuller(
    const Matrix4f& mat, const Matrix4f& matInv) {
  auto& vol = mPosition.volume();
  return std::make_shared<VolumeCuller>(
      mProjModelView * mat, om::coords::Norm(matInv * mPosition, vol),
      om::coords::Norm(matInv * mFocus, vol));
}

Visibility VolumeCuller::TestChunk(const om::coords::NormBbox& normBox) const {
  return mFrustumCuller.testAabb(normBox);
}

bool matrciesAreSame(const Matrix3<float>& a, const Matrix3<float>& b) {
  for (auto i = 0; i < 9; ++i) {
    if (!qFuzzyCompare(a.ml[i], b.ml[i])) {
      return false;
    }
  }
  return true;
}

bool matrciesAreSame(const Matrix4<float>& a, const Matrix4<float>& b) {
  for (auto i = 0; i < 16; ++i) {
    if (!qFuzzyCompare(a.ml[i], b.ml[i])) {
      return false;
    }
  }
  return true;
}

bool vecSame(const Vector3<float>& a, const Vector3<float>& b) {
  return qFuzzyCompare(a.x, b.x) && qFuzzyCompare(a.y, b.y) &&
         qFuzzyCompare(a.z, b.z);
}

bool VolumeCuller::operator==(const VolumeCuller& c) const {
  return matrciesAreSame(mProjModelView, c.mProjModelView) &&
         vecSame(mPosition, c.mPosition) && vecSame(mFocus, c.mFocus);
}

bool VolumeCuller::operator!=(const VolumeCuller& c) const {
  return !(*this == c);
}
}
}  //namespace
