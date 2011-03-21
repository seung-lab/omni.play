#ifndef OM_BRUSH_SELECT_CIRCLE_HPP
#define OM_BRUSH_SELECT_CIRCLE_HPP

#include "view2d/brush/omBrushSelectUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushSelectCircle {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;

public:
    OmBrushSelectCircle(boost::shared_ptr<OmBrushOppInfo> info)
        : info_(info)
    {}

    virtual ~OmBrushSelectCircle()
    {}

    void SelectCircle(const DataCoord& coord)
    {
        OmBrushOppCircle circleOpp(info_);

        boost::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

        boost::shared_ptr<boost::unordered_set<OmSegID> > segIDs =
            OmBrushSelectUtils::FindSegIDsFromPoints(info_.get(), pts.get());

        OmBrushSelectUtils::SendEvent(info_.get(), segIDs.get());
    }
};

#endif
