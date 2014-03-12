#pragma once

#include "chunks/omChunkDataInterface.hpp"
#include "utility/dataWrappers.h"
#include "chunks/omChunk.h"

class OmCompareVolumes {
 public:
  static bool CompareChannels(const om::common::ID id1,
                              const om::common::ID id2) {
    OmChannel& chan1 = ChannelDataWrapper(id1).GetChannel();
    OmChannel& chan2 = ChannelDataWrapper(id2).GetChannel();
    return compareVolumes(&chan1, &chan2);
  }

  static bool CompareSegmentations(const om::common::ID id1,
                                   const om::common::ID id2) {
    OmSegmentation* seg1 = SegmentationDataWrapper(id1).GetSegmentation();
    OmSegmentation* seg2 = SegmentationDataWrapper(id2).GetSegmentation();
    return compareVolumes(seg1, seg2);
  }

 private:
  /*
   * Returns true if two given volumes are exactly the same.
   * Prints all positions where volumes differ if verbose flag is set.
   */
  template <typename VOL>
  static bool compareVolumes(VOL* vol1, VOL* vol2) {
    // check if dimensions are the same
    if (vol1->Coords().Extent().getUnitDimensions() !=
        vol2->Coords().Extent().getUnitDimensions()) {
      log_infos << "Volumes differ: Different dimensions.";
      return false;
    }

    // root mip level should be the same if data dimensions are the same
    if (vol1->Coords().RootMipLevel() != vol2->Coords().RootMipLevel()) {
      log_infos << "Volumes differ: Different number of MIP levels.";
      return false;
    }

    for (int level = 0; level <= vol1->Coords().RootMipLevel(); ++level) {
      log_infos << "Comparing mip level " << level;

      std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
          vol1->GetMipChunkCoords(level);

      FOR_EACH(iter, *coordsPtr) {
        const om::coords::Chunk& coord = *iter;

        if (CompareChunks(coord, vol1, vol2)) {
          continue;
        }

        log_infos << "\tchunks differ at " << coord << "; aborting...";
        return false;
      }
    }

    return true;
  }

  /*
   * Returns true if two given chunks contain the exact same image data
   */
  template <typename VOL>
  static bool CompareChunks(const om::coords::Chunk& coord, VOL* vol1,
                            VOL* vol2) {
    OmChunk* chunk1 = vol1->GetChunk(coord);

    OmChunk* chunk2 = vol2->GetChunk(coord);

    if (chunk1->GetCoordinate() != chunk2->GetCoordinate()) {
      log_infos << "Chunks differ: Different coords.";
      return false;
    }

    if (chunk1->GetDimensions() != chunk2->GetDimensions()) {
      log_infos << "Chunks differ: Different dimensions.";
      return false;
    }

    return chunk1->Data()->Compare(chunk2->Data());
  }
};
