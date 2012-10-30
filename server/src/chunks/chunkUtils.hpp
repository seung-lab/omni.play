#pragma once

#include "utility/image.hpp"
#include "volume/volume.h"

namespace om {
namespace chunks {    

class utils {
public:
    /**
     *      Returns new utility::image containing the entire extent of data needed
     *      to form continuous meshes with adjacent MipChunks.  This means an extra
     *      voxel of data is included on each dimensions.
     */
    static utility::image<uint32_t, 3> GetMeshImageData(const volume::volume& vol,
                                                        const coords::Chunk& chunk)
    {
        utility::image<uint32_t, 3> retImage(utility::extents[129][129][129]);

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
                    const coords::Chunk mip_coord(chunk.getLevel(),
                                                  chunk.x + x,
                                                  chunk.y + y,
                                                  chunk.z + z);

                    //skip invalid mip coord
                    if(vol.CoordSystem().ContainsMipChunk(mip_coord))
                    {
                        const boost::shared_ptr<uint32_t> rawDataPtr = 
                            vol.GetChunk<uint32_t>(mip_coord);

                        utility::image<uint32_t, 3> chunkImage(utility::extents[128][128][128],
                                                        rawDataPtr.get());

                        retImage.copyFrom( chunkImage,
                                           utility::extents[z*128][y*128][x*128],
                                           utility::extents[0][0][0],
                                           utility::extents[lenZ][lenY][lenX] );
                    }
                }
            }
        }

        return retImage;
    }
};

}} // namespace om::chunks