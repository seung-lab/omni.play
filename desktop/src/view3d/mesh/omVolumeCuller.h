#pragma once

/*
 * A FrustumCuller wrapper specifically used to cull objects in a volume.
 * Rather than extracting the
 * projection and modelview matrix for each transformed frame, the VolumeCuller
 * can return
 * a new transformed culler given a matrix and it's inverse (these should be
 * precomputed).
 *
 * The culler also keeps track of it's position of the camera and camera's
 * focus.
 */

#include "common/common.h"
#include "coordinates/coordinates.h"

class OmVolumeCuller {
 public:
  OmVolumeCuller(const Matrix4f& projmodelview, const om::coords::Norm& pos,
                 const om::coords::Norm& focus);

  Visibility TestChunk(const om::coords::NormBbox&) const;
  const om::coords::Norm& GetPosition() const;

  std::shared_ptr<OmVolumeCuller> GetTransformedCuller(const Matrix4f&,
                                                       const Matrix4f&);

  bool operator==(const OmVolumeCuller& c) const;
  bool operator!=(const OmVolumeCuller& c) const;

 private:
  const Matrix4f mProjModelView;
  const om::coords::Norm mPosition;
  const om::coords::Norm mFocus;

  FrustumCullerf mFrustumCuller;
};
