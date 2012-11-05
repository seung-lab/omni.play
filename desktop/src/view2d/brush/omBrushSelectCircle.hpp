#pragma once

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

    void SelectCircle(const om::coords::Global& coord)
    {
        OmBrushOppCircle circleOpp(info_);

        boost::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

//         std::cout << "coord: " << coord << "\n";
//         FOR_EACH(iter, *pts){
//             std::cout << "\t" << *iter << "\n";
//         }

        boost::shared_ptr<boost::unordered_set<om::common::SegID> > segIDs =
            OmBrushSelectUtils::FindSegIDsFromPoints(info_.get(), pts.get());

        OmBrushSelectUtils::SendEvent(info_.get(), segIDs.get());
    }
};

