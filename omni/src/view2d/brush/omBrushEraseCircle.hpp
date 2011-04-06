#ifndef OM_BRUSH_ERASE_CIRCLE_HPP
#define OM_BRUSH_ERASE_CIRCLE_HPP

#include "view2d/brush/omBrushEraseUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushEraseCircle  {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;
    const OmSegID segIDtoErase_;

public:
    OmBrushEraseCircle(boost::shared_ptr<OmBrushOppInfo> info,
                       const OmSegID segIDtoErase)
        : info_(info)
        , segIDtoErase_(segIDtoErase)
    {}

    virtual ~OmBrushEraseCircle()
    {}

    void EraseCircle(const DataCoord& coord)
    {
        OmBrushOppCircle circleOpp(info_);

        boost::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

        OmBrushEraseUtils::ErasePts(info_.get(), pts.get(), segIDtoErase_);
    }
};

#endif
