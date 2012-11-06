#pragma once

#include "view2d/omSliceCache.hpp"

class OmChunksAndPts {
private:
    OmSegmentation *const vol_;
    const om::common::ViewType viewType_;

    std::set<om::coords::Data> pts_;

public:
    OmChunksAndPts(OmSegmentation* vol, const om::common::ViewType viewType)
        : vol_(vol)
        , viewType_(viewType)
    {}

    void AddAllPtsThatIntersectVol(om::pt3d_list_t* pts)
    {
        const om::coords::GlobalBbox& segDataExtent = vol_->Coords().Extent();

        FOR_EACH(iter, *pts)
        {
            if(!segDataExtent.contains(*iter)){
                continue;
            }

            om::coords::Data coord = iter->toDataCoord(vol_, 0);

            pts_.insert(coord);
        }
    }

	boost::shared_ptr<boost::unordered_set<om::common::SegID> >
    GetSegIDs()
    {
        OmSliceCache sliceCache(vol_, viewType_);

        boost::shared_ptr<boost::unordered_set<om::common::SegID> > ret =
            boost::make_shared<boost::unordered_set<om::common::SegID> >();

        FOR_EACH(pt, pts_) {
        	om::common::SegID id = sliceCache.GetVoxelValue(*pt);
            if(id) {
            	ret->insert(id);
            }
        }

        return ret;
    }
};

