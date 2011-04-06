#ifndef OM_CHUNK_UTILS_HPP
#define OM_CHUNK_UTILS_HPP

#include "chunks/omChunkCoord.h"
#include "chunks/omSegChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "utility/image/omImage.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmChunkUtils{
public:
    static void	RewriteChunkAtThreshold(OmSegmentation* segmentation,
                                        OmImage< uint32_t, 3 >& chunkData,
                                        const double threshold)
    {
        if(qFuzzyCompare(1, threshold)){
            return;
        }

        OmSegments* segments = segmentation->Segments();
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
    static OmImage<uint32_t, 3> GetMeshOmImageData(OmSegmentation* vol,
                                                   OmSegChunkPtr chunk)
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
                    const OmChunkCoord& currentCoord = chunk->GetCoordinate();

                    const OmChunkCoord mip_coord(currentCoord.getLevel(),
                                                 currentCoord.getCoordinateX() + x,
                                                 currentCoord.getCoordinateY() + y,
                                                 currentCoord.getCoordinateZ() + z);

                    //skip invalid mip coord
                    if(vol->Coords().ContainsMipChunkCoord(mip_coord))
                    {
                        OmSegChunkPtr chunk;
                        vol->GetChunk(chunk, mip_coord);

                        boost::shared_ptr<uint32_t> rawDataPtr =
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

    static void RefindUniqueChunkValues(const OmID segmentationID_)
    {
        SegmentationDataWrapper sdw(segmentationID_);
        OmSegmentation& vol = sdw.GetSegmentation();

        boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
            vol.GetMipChunkCoords();

        const uint32_t numChunks = coordsPtr->size();

        int counter = 0;

        FOR_EACH(iter, *coordsPtr)
        {
            const OmChunkCoord& coord = *iter;

            ++counter;
            printf("\rfinding values in chunk %d of %d...", counter, numChunks);
            fflush(stdout);

            vol.ChunkUniqueValues()->RereadChunk(coord, 1);
        }
    }
};

#endif
