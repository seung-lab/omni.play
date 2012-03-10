#pragma once

#include "events/omEvents.h"
#include "actions/omActions.h"
#include "tiles/cache/omTileCache.h"
#include "view2d/brush/omBrushOppTypes.h"

#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmBrushPaintUtils {
public:
    static void PaintPts(OmBrushOppInfo* info, om::pt3d_list_t* pts,
                         const OmSegID segIDtoPaint)
    {
        const DataBbox& segDataExtent = info->segmentation->Coords().GetDataExtent();

        std::set<Vector3i> voxelCoords;

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
                             segIDtoPaint);

        removeModifiedTiles(info, voxelCoords);

        OmEvents::Redraw2dBlocking();
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
            OmRawSegTileCache* sliceCache = seg.SliceCache();
            sliceCache->Clear();
        }

        OmTileCache::ClearSegmentation();
    }
};

