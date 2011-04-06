#ifndef OM_BRUSH_ERASE_UTILS_HPP
#define OM_BRUSH_ERASE_UTILS_HPP

#include "events/omEvents.h"
#include "actions/omActions.h"
#include "tiles/cache/omTileCache.h"
#include "view2d/brush/omBrushOppTypes.h"

#include "system/cache/omVolSliceCache.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmBrushEraseUtils {
public:
    static void ErasePts(OmBrushOppInfo* info, om::pt3d_list_t* pts,
                         const OmSegID segIDtoErase)
    {
        const DataBbox& segDataExtent = info->segmentation->Coords().GetDataExtent();

        std::set<Vector3i> voxelCoords;

        OmChunksAndPts chunksAndPts(info->segmentation);

        FOR_EACH(iter, *pts)
        {
            if(!segDataExtent.contains(*iter)){
                continue;
            }

            chunksAndPts.AddPtAbsVoxelLoc(*iter);
        }

        OmSliceCache sliceCache(info->segmentation,
                                info->viewType);

        virtual boost::shared_ptr<boost::unordered_set<OmSegID> > segIDsAndPts =
            sliceCache.GetSegIDs(chunksAndPts, info->depth % info->chunkDim);


        FOR_EACH(iter, *pts)
        {
            if(!segDataExtent.contains(*iter)){
                continue;
            }

            voxelCoords.insert(*iter);
        }

        if(!voxelCoords.size()){
            return;
        }

        OmActions::SetVoxels(info->segmentation->GetID(),
                             voxelCoords,
                             segIDtoErase);

        removeModifiedTiles(info, voxelCoords);

        OmEvents::Redraw2d();
    }

private:
    static void removeModifiedTiles(OmBrushOppInfo*,
                                    const std::set<DataCoord>&)
    {
        // const int chunkDim = info->chunkDim;

        // std::map<OmChunkCoord, std::set<Vector3i> > ptsInChunks;

        // FOR_EACH(iter, voxelCoords)
        // {
        //     const OmChunkCoord chunkCoord(0,
        //                                   iter->x / chunkDim,
        //                                   iter->y / chunkDim,
        //                                   iter->z / chunkDim);

        //     const Vector3i chunkPos(iter->x % chunkDim,
        //                             iter->y % chunkDim,
        //                             iter->z % chunkDim);


        //     OmTileCache::RemoveDataCoord(Vector3i(iter->x / chunkDim,
        //                                           iter->y / chunkDim,
        //                                           iter->z % chunkDim));

        //     ptsInChunks[chunkCoord].insert(chunkPos);
        // }

        const OmIDsSet& segset = SegmentationDataWrapper::ValidIDs();

        FOR_EACH(iter, segset)
        {
            SegmentationDataWrapper sdw(*iter);
            OmSegmentation& seg = sdw.GetSegmentation();
            OmVolSliceCache* sliceCache = seg.SliceCache();
            sliceCache->Clear();
        }

        OmTileCache::ClearSegmentation();
    }
};

#endif
