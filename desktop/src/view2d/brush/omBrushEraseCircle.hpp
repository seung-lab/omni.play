#pragma once

#include "view2d/brush/omBrushEraseUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushEraseCircle  {
private:
    const om::shared_ptr<OmBrushOppInfo> info_;
    const OmSegID segIDtoErase_;

public:
    OmBrushEraseCircle(om::shared_ptr<OmBrushOppInfo> info,
                       const OmSegID segIDtoErase)
        : info_(info)
        , segIDtoErase_(segIDtoErase)
    {}

    virtual ~OmBrushEraseCircle()
    {}

    void EraseCircle(const om::coords::Global& coord)
    {
        OmBrushOppCircle circleOpp(info_);

        om::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

        OmBrushEraseUtils::ErasePts(info_.get(), pts.get(), segIDtoErase_);
    }
};

