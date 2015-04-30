#pragma once
#include "precomp.h"

#include "utility/image/omImage.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegments.h"
#include "chunk/chunkUtils.hpp"
#include "volume/omSegmentation.h"

class OmChunkUtils {
 public:
  static void RewriteChunkAtThreshold(OmSegmentation* segmentation,
                                      OmImage<uint32_t, 3>& chunkData,
                                      const double threshold) {
    if (qFuzzyCompare(1, threshold)) {
      return;
    }

    auto& segments = segmentation->Segments();
    segmentation->SetDendThreshold(threshold);

    uint32_t* rawData = chunkData.getScalarPtrMutate();

    for (size_t i = 0; i < chunkData.size(); ++i) {
      if (0 != rawData[i]) {
        rawData[i] = segments.FindRootID(rawData[i]);
      }
    }
  }

  /**
   *      Returns new OmImage containing the entire extent of data needed
   *      to form continuous meshes with adjacent MipChunks.  This means an
   * extra
   *      voxel of data is included on each dimensions.
   */
  static OmImage<uint32_t, 3> GetMeshOmImageData(
      OmSegmentation* vol, const om::coords::Chunk& chunkCoord) {
    OmImage<uint32_t, 3> retImage(OmExtents[129][129][129]);

    for (int z = 0; z < 2; ++z) {
      for (int y = 0; y < 2; ++y) {
        for (int x = 0; x < 2; ++x) {
          const int lenZ = z ? 1 : 128;
          const int lenY = y ? 1 : 128;
          const int lenX = x ? 1 : 128;

          const om::coords::Chunk mip_coord(chunkCoord.mipLevel(),
                                            chunkCoord.x + x, chunkCoord.y + y,
                                            chunkCoord.z + z);

          // skip invalid mip coord
          if (vol->Coords().ContainsMipChunk(mip_coord)) {
            auto chunk = vol->GetChunk(mip_coord);
            auto typedChunk =
                boost::get<om::chunk::Chunk<uint32_t>>(chunk.get());
            if (!typedChunk) {
              continue;
            }

            OmImage<uint32_t, 3> chunkImage(OmExtents[128][128][128],
                                            typedChunk->data().get());

            retImage.copyFrom(chunkImage, OmExtents[z * 128][y * 128][x * 128],
                              OmExtents[0][0][0], OmExtents[lenZ][lenY][lenX]);
          }
        }
      }
    }

    return retImage;
  }

  static void RefindUniqueChunkValues(const om::common::ID segmentationID_) {
    SegmentationDataWrapper sdw(segmentationID_);
    if (!sdw.IsValidWrapper()) {
      return;
    }

    OmSegmentation& vol = *sdw.GetSegmentation();

    auto coordsPtr = vol.GetMipChunkCoords();
    const uint32_t numChunks = coordsPtr->size();

    int counter = 0;

    for (auto& coord : *coordsPtr) {
      ++counter;
      log_info("\rfinding values in chunk %d of %d...", counter, numChunks);

      auto uv = om::chunk::utils::MakeUniqueValues(vol.ChunkDS(), coord,
                                                   vol.Coords());
      vol.UniqueValuesDS().Put(coord, uv);
    }
  }
};
