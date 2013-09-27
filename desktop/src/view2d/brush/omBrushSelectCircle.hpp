#pragma once

#include "view2d/brush/omBrushSelectUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushSelectCircle {
private:
    const std::shared_ptr<OmBrushOppInfo> info_;

public:
    OmBrushSelectCircle(std::shared_ptr<OmBrushOppInfo> info)
        : info_(info)
    {}

    virtual ~OmBrushSelectCircle()
    {}

    void SelectCircle(const om::globalCoord& coord)
    {
        OmBrushOppCircle circleOpp(info_);

        std::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

//         std::cout << "coord: " << coord << "\n";
//         FOR_EACH(iter, *pts){
//             std::cout << "\t" << *iter << "\n";
//         }

        std::shared_ptr<std::unordered_set<om::common::SegID> > segIDs =
            OmBrushSelectUtils::FindSegIDsFromPoints(info_.get(), pts.get());

        OmBrushSelectUtils::SendEvent(info_.get(), segIDs.get());
    }
};

