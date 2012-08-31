#pragma once

#include "view2d/omSliceCache.hpp"

class OmChunksAndPts {
private:
    OmSegmentation *const vol_;
    const ViewType viewType_;

    std::set<om::dataCoord> pts_;

public:
    OmChunksAndPts(OmSegmentation* vol, const ViewType viewType)
        : vol_(vol)
        , viewType_(viewType)
    {}

    void AddAllPtsThatIntersectVol(om::pt3d_list_t* pts)
    {
        const om::globalBbox& segDataExtent = vol_->Coords().GetDataExtent();

        FOR_EACH(iter, *pts)
        {
            if(!segDataExtent.contains(*iter)){
                continue;
            }

            om::dataCoord coord = iter->toDataCoord(vol_, 0);

            pts_.insert(coord);
        }
    }

	om::shared_ptr<boost::unordered_set<OmSegID> >
    GetSegIDs()
    {
        OmSliceCache sliceCache(vol_, viewType_);

        om::shared_ptr<boost::unordered_set<OmSegID> > ret =
            om::make_shared<boost::unordered_set<OmSegID> >();

        FOR_EACH(pt, pts_) {
            ret->insert(sliceCache.GetVoxelValue(*pt));
        }

        return ret;
    }
};

