#pragma once

#include "view2d/brush/omBrushOppLine.hpp"
#include "view2d/brush/omBrushEraseUtils.hpp"

class OmBrushEraseLine {
private:
    const std::shared_ptr<OmBrushOppInfo> info_;
    const om::common::SegID segIDtoErase_;

public:
    OmBrushEraseLine(std::shared_ptr<OmBrushOppInfo> info,
                     const om::common::SegID segIDtoErase)
        : info_(info)
        , segIDtoErase_(segIDtoErase)
    {}

    virtual ~OmBrushEraseLine()
    {}

    void EraseLine(const om::globalCoord& first, const om::globalCoord& second)
    {
        OmBrushOppLine lineOpp(info_);

        std::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);

        OmBrushEraseUtils::ErasePts(info_.get(), pts.get(), segIDtoErase_);
    }
};

