#ifndef OM_BRUSH_ERASE_LINE_HPP
#define OM_BRUSH_ERASE_LINE_HPP

#include "view2d/brush/omBrushOppLine.hpp"
#include "view2d/brush/omBrushEraseUtils.hpp"

class OmBrushEraseLine {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;
    const OmSegID segIDtoErase_;

public:
    OmBrushEraseLine(boost::shared_ptr<OmBrushOppInfo> info,
                     const OmSegID segIDtoErase)
        : info_(info)
        , segIDtoErase_(segIDtoErase)
    {}

    virtual ~OmBrushEraseLine()
    {}

    void EraseLine(const DataCoord& first, const DataCoord& second)
    {
        OmBrushOppLine lineOpp(info_);

        boost::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);

        OmBrushEraseUtils::ErasePts(info_.get(), pts.get(), segIDtoErase_);
    }
};

#endif
