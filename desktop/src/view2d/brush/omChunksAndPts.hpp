#pragma once

#include "view2d/omSliceCache.hpp"

class OmChunksAndPts {
private:
    OmSegmentation *const vol_;
    const ViewType viewType_;

    std::map<om::chunkCoord, std::set<om::dataCoord> > ptsInChunks_;

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

            ptsInChunks_[coord.toChunkCoord()].insert(coord);
        }
    }

    om::shared_ptr<boost::unordered_set<OmSegID> >
    GetSegIDs(const int depth)
    {
        OmSliceCache sliceCache(vol_, viewType_);

        om::shared_ptr<boost::unordered_set<OmSegID> > ret =
            om::make_shared<boost::unordered_set<OmSegID> >();

        FOR_EACH(iter, ptsInChunks_)
        {
            const om::chunkCoord& coord = iter->first;
            
            PooledTile32Ptr slicePtr = sliceCache.GetSlice(coord, depth);
            uint32_t const*const sliceData = slicePtr->GetData();

            const std::set<om::dataCoord>& pts = iter->second;

            FOR_EACH(vec, pts)
            {
                const OmSegID segID = sliceData[vec->toTileOffset(viewType_)];

                if(segID){
                    ret->insert(segID);
                }
            }
        }

        return ret;
    }
};

