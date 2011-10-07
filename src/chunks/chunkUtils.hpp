#pragma once

#include "chunks/segChunk.h"
#include "chunks/uniqueValues/chunkUniqueValuesManager.hpp"
#include "utility/image/omImage.hpp"
#include "utility/segmentationDataWrapper.hpp"

class chunkUtils{
public:
    static void	RewriteChunkAtThreshold(segmentation* segmentation,
                                        OmImage< uint32_t, 3 >& chunkData,
                                        const double threshold)
    {
        if(qFuzzyCompare(1, threshold)){
            return;
        }

        segments* segments = segmentation->Segments();
        segmentation->SetDendThreshold(threshold);

        uint32_t* rawData = chunkData.getScalarPtrMutate();

        for(size_t i = 0; i < chunkData.size(); ++i){
            if( 0 != rawData[i]) {
                rawData[i] = segments->findRootID(rawData[i]);
            }
        }
    }

    /**
     *      Returns new OmImage containing the entire extent of data needed
     *      to form continuous meshes with adjacent MipChunks.  This means an extra
     *      voxel of data is included on each dimensions.
     */
    static OmImage<uint32_t, 3> GetMeshOmImageData(segmentation* vol,
                                                   segChunk* chunk)
    {
        OmImage<uint32_t, 3> retImage(OmExtents[129][129][129]);

        for ( int z = 0; z < 2; ++z )
        {
            for ( int y = 0; y < 2; ++y )
            {
                for ( int x = 0; x < 2; ++x )
                {
                    const int lenZ = z ? 1 : 128;
                    const int lenY = y ? 1 : 128;
                    const int lenX = x ? 1 : 128;

                    //form mip coord
                    const coords::chunkCoord& currentCoord = chunk->GetCoordinate();

                    const coords::chunkCoord mip_coord(currentCoord.getLevel(),
                                                   currentCoord.X() + x,
                                                   currentCoord.Y() + y,
                                                   currentCoord.Z() + z);

                    //skip invalid mip coord
                    if(vol->Coords().ContainsMipChunkCoord(mip_coord))
                    {
                        segChunk* chunk = vol->GetChunk(mip_coord);

                        om::shared_ptr<uint32_t> rawDataPtr =
                            chunk->SegData()->GetCopyOfChunkDataAsUint32();

                        OmImage<uint32_t, 3> chunkImage(OmExtents[128][128][128],
                                                        rawDataPtr.get());

                        retImage.copyFrom( chunkImage,
                                           OmExtents[z*128][y*128][x*128],
                                           OmExtents[0][0][0],
                                           OmExtents[lenZ][lenY][lenX] );
                    }
                }
            }
        }

        return retImage;
    }

    static void RefindUniqueChunkValues(const common::id segmentationID_)
    {
        SegmentationDataWrapper sdw(segmentationID_);
        segmentation& vol = sdw.GetSegmentation();

        om::shared_ptr<std::deque<coords::chunkCoord> > coordsPtr =
            vol.GetMipChunkCoords();

        const uint32_t numChunks = coordsPtr->size();

        int counter = 0;

        FOR_EACH(iter, *coordsPtr)
        {
            const coords::chunkCoord& coord = *iter;

            ++counter;
            printf("\rfinding values in chunk %d of %d...", counter, numChunks);
            fflush(stdout);

            vol.ChunkUniqueValues()->RereadChunk(coord, 1);
        }
    }
};

