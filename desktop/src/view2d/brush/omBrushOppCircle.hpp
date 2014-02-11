#pragma once

#include "view2d/brush/omBrushOppUtils.hpp"

class OmBrushOppCircle {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const int brushDia_;
  const int depth_;
  const om::common::ViewType viewType_;

 public:
  OmBrushOppCircle(std::shared_ptr<OmBrushOppInfo> info)
      : info_(info),
        brushDia_(info_->brushDia),
        depth_(info_->depth),
        viewType_(info_->viewType) {}

  virtual ~OmBrushOppCircle() {}

  std::shared_ptr<om::pt3d_list_t> GetPts(const om::coords::Global& xyzCoord) {
    std::shared_ptr<om::pt3d_list_t> ret = std::make_shared<om::pt3d_list_t>();

    OmBrushOppUtils::GetPts(info_.get(), ret.get(), xyzCoord, viewType_);

    return ret;
  }
};
