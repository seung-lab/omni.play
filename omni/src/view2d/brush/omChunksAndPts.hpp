#ifndef OM_PTS_IN_CHUNK_HPP
#define OM_PTS_IN_CHUNK_HPP

#include "view2d/omSliceCache.hpp"

class OmChunksAndPts {
private:
    OmSegmentation *const vol_;
    const ViewType viewType_;
    const int chunkDim_;

    std::map<OmChunkCoord, std::set<Vector3i> > ptsInChunks_;

public:
    OmChunksAndPts(OmSegmentation* vol, const ViewType viewType)
        : vol_(vol)
        , viewType_(viewType)
        , chunkDim_(vol->Coords().GetChunkDimension())
    {}

    void AddAllPtsThatIntersectVol(om::pt3d_list_t* pts)
    {
        const DataBbox& segDataExtent = vol_->Coords().GetDataExtent();

        FOR_EACH(iter, *pts)
        {
            if(!segDataExtent.contains(*iter)){
                continue;
            }

            AddPtAbsVoxelLoc(*iter);
        }
    }

    inline void AddPtAbsVoxelLoc(const Vector3i& vec)
    {
        const OmChunkCoord chunkCoord(0,
                                      vec.x / chunkDim_,
                                      vec.y / chunkDim_,
                                      vec.z / chunkDim_);

        const Vector3i chunkPos(vec.x % chunkDim_,
                                vec.y % chunkDim_,
                                vec.z % chunkDim_);


        ptsInChunks_[chunkCoord].insert(chunkPos);
    }

    om::shared_ptr<boost::unordered_set<OmSegID> >
    GetSegIDs(const int depth)
    {
        OmSliceCache sliceCache(vol_, viewType_);

        om::shared_ptr<boost::unordered_set<OmSegID> > ret =
            om::make_shared<boost::unordered_set<OmSegID> >();

        FOR_EACH(iter, ptsInChunks_)
        {
            const OmChunkCoord& coord = iter->first;

            om::shared_ptr<uint32_t> slicePtr = sliceCache.GetSlice(coord, depth);
            uint32_t const*const sliceData = slicePtr.get();

            const std::set<Vector3i>& pts = iter->second;

            FOR_EACH(vec, pts)
            {
                const Vector2i loc =
                    OmView2dConverters::Get2PtsInPlane(*vec, viewType_);

                OmSegID segID = 0;
                if(ZY_VIEW == viewType_){
                    segID = sliceData[chunkDim_ * loc.x + loc.y];
                } else {
                    segID = sliceData[chunkDim_ * loc.y + loc.x];
                }

                if(segID){
                    ret->insert(segID);
                }
            }
        }

        return ret;
    }
};

#endif
