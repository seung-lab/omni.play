#ifndef OM_BRUSH_PAINT_LINE_HPP
#define OM_BRUSH_PAINT_LINE_HPP

#include "view2d/brush/omBrushOppLine.hpp"
#include "view2d/brush/omBrushPaintUtils.hpp"

class OmBrushPaintLine {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;
    const OmSegID segIDtoPaint_;

public:
    OmBrushPaintLine(boost::shared_ptr<OmBrushOppInfo> info,
                     const OmSegID segIDtoPaint)
        : info_(info)
        , segIDtoPaint_(segIDtoPaint)
    {}

    virtual ~OmBrushPaintLine()
    {}

    void PaintLine(const DataCoord& first, const DataCoord& second)
    {
        OmBrushOppLine lineOpp(info_);

        boost::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);

        OmBrushPaintUtils::PaintPts(info_.get(), pts.get(), segIDtoPaint_);
    }
};

#endif
