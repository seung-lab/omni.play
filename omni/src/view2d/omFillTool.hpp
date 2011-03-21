#ifndef OM_FILL_TOOL_HPP
#define OM_FILL_TOOL_HPP

#include "view2d/omView2dConverters.hpp"
#include "volume/omSegmentation.h"

class OmFillTool {
private:
    OmSegmentation *const segmentation_;
    OmSegments *const segments_;
    const ViewType viewType_;
    const OmSegID fc_;
    const OmSegID bc_;

    inline DataCoord flipCoords(const DataCoord& vec){
        return OmView2dConverters::MakeViewTypeVector3(vec, viewType_);
    }

public:
    OmFillTool(OmSegmentation* segmentation, const ViewType viewType,
               const OmSegID fc, const OmSegID bc)
        : segmentation_(segmentation)
        , segments_(segmentation->Segments())
        , viewType_(viewType)
        , fc_(fc)
        , bc_(bc)
    {}

    void Fill(DataCoord gCP, int depth = 0)
    {
        OmID segid = segmentation_->GetVoxelValue(gCP);

        if (!segid){
            return;
        }

        segid = segments_->findRootID(segid);

        if (depth > 5000){
            return;
        }

        ++depth;

        if(segid == bc_ && segid != fc_) {

            DataCoord off = flipCoords(gCP);

            //(new OmVoxelSetValueAction(seg, gCP, fc))->Run();

            off.x++;
            Fill(flipCoords(off), depth);
            off.y++;
            Fill(flipCoords(off), depth);
            off.x--;
            Fill(flipCoords(off), depth);
            off.x--;
            Fill(flipCoords(off), depth);
            off.y--;
            Fill(flipCoords(off), depth);
            off.y--;
            Fill(flipCoords(off), depth);
            off.x++;
            Fill(flipCoords(off), depth);
            off.x++;
            Fill(flipCoords(off), depth);
        }
    }
};

#endif
