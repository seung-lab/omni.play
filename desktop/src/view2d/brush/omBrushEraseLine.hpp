#pragma once

#include "view2d/brush/omBrushOppLine.hpp"
#include "view2d/brush/omBrushEraseUtils.hpp"

class OmBrushEraseLine {
private:
    const om::shared_ptr<OmBrushOppInfo> info_;
    const om::common::SegID segIDtoErase_;

public:
    OmBrushEraseLine(om::shared_ptr<OmBrushOppInfo> info,
                     const om::common::SegID segIDtoErase)
        : info_(info)
        , segIDtoErase_(segIDtoErase)
    {}

    virtual ~OmBrushEraseLine()
    {}

    void EraseLine(const om::coords::Global& first, const om::coords::Global& second)
    {
        OmBrushOppLine lineOpp(info_);

        om::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);

        OmBrushEraseUtils::ErasePts(info_.get(), pts.get(), segIDtoErase_);
    }
};

