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

#include "coordinates/normCoord.h"
#include "common/common.h"
#include "omDrawOptions.h"

class OmVolumeCuller {
 public:
  OmVolumeCuller(const Matrix4f& projmodelview, const om::normCoord& pos,
                 const om::normCoord& focus);

  Visibility TestChunk(const om::normBbox&) const;
  const om::normCoord& GetPosition() const;

  std::shared_ptr<OmVolumeCuller> GetTransformedCuller(const Matrix4f&,
                                                       const Matrix4f&);

  bool operator==(const OmVolumeCuller& c) const;
  bool operator!=(const OmVolumeCuller& c) const;
  bool equals(const std::shared_ptr<OmVolumeCuller>& c) { return *this == *c; }

 private:
  const Matrix4f mProjModelView;
  const om::normCoord mPosition;
  const om::normCoord mFocus;

  FrustumCullerf mFrustumCuller;
};
