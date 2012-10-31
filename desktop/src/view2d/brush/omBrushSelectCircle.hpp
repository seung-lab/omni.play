#pragma once

#include "view2d/brush/omBrushSelectUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushSelectCircle {
private:
    const om::shared_ptr<OmBrushOppInfo> info_;

public:
    OmBrushSelectCircle(om::shared_ptr<OmBrushOppInfo> info)
        : info_(info)
    {}

    virtual ~OmBrushSelectCircle()
    {}

    void SelectCircle(const om::coords::Global& coord)
    {
        OmBrushOppCircle circleOpp(info_);

        om::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

//         std::cout << "coord: " << coord << "\n";
//         FOR_EACH(iter, *pts){
//             std::cout << "\t" << *iter << "\n";
//         }

        om::shared_ptr<boost::unordered_set<OmSegID> > segIDs =
            OmBrushSelectUtils::FindSegIDsFromPoints(info_.get(), pts.get());

        OmBrushSelectUtils::SendEvent(info_.get(), segIDs.get());
    }
};

