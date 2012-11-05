#pragma once

#include "view2d/brush/omBrushPaintUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushPaintCircle  {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;
    const om::common::SegID segIDtoPaint_;

public:
    OmBrushPaintCircle(boost::shared_ptr<OmBrushOppInfo> info,
                       const om::common::SegID segIDtoPaint)
        : info_(info)
        , segIDtoPaint_(segIDtoPaint)
    {}

    virtual ~OmBrushPaintCircle()
    {}

    void PaintCircle(const om::coords::Global& coord)
    {
        OmBrushOppCircle circleOpp(info_);

        boost::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

        OmBrushPaintUtils::PaintPts(info_.get(), pts.get(), segIDtoPaint_);
    }
};

