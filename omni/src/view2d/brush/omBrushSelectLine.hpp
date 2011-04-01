#ifndef OM_BRUSH_SELECT_LINE_HPP
#define OM_BRUSH_SELECT_LINE_HPP

#include "view2d/brush/omBrushOppLine.hpp"
#include "view2d/brush/omBrushOppTypes.h"
#include "view2d/omSliceCache.hpp"
#include "segment/omSegmentSelector.h"

class OmBrushSelectLine {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;

public:
    OmBrushSelectLine(boost::shared_ptr<OmBrushOppInfo> info)
        : info_(info)
    {}

    virtual ~OmBrushSelectLine()
    {}

    void SelectLine(const DataCoord& first, const DataCoord& second)
    {
        OmBrushOppLine lineOpp(info_);

        boost::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);

        boost::shared_ptr<boost::unordered_set<OmSegID> > segIDs =
            OmBrushSelectUtils::FindSegIDsFromPoints(info_.get(), pts.get());

        OmBrushSelectUtils::SendEvent(info_.get(), segIDs.get());
    }
};

#endif
