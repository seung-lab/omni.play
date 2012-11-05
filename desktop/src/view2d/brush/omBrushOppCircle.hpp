#pragma once

#include "view2d/brush/omBrushOppUtils.hpp"

class OmBrushOppCircle {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;
    const int brushDia_;
    const int depth_;
    const ViewType viewType_;

public:
    OmBrushOppCircle(boost::shared_ptr<OmBrushOppInfo> info)
        : info_(info)
        , brushDia_(info_->brushDia)
        , depth_(info_->depth)
        , viewType_(info_->viewType)
    {}

    virtual ~OmBrushOppCircle()
    {}

    boost::shared_ptr<om::pt3d_list_t>
    GetPts(const om::coords::Global& xyzCoord)
    {
        boost::shared_ptr<om::pt3d_list_t> ret =
            om::make_shared<om::pt3d_list_t>();

        OmBrushOppUtils::GetPts(info_.get(), ret.get(), xyzCoord, viewType_);

        return ret;
    }
};

