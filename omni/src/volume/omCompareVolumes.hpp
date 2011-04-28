#ifndef OM_COMPARE_VOLUMES_HPP
#define OM_COMPARE_VOLUMES_HPP

#include "utility/dataWrappers.h"
#include "chunks/omChunk.h"

class OmCompareVolumes {
public:
    static bool CompareChannels(const OmID id1, const OmID id2)
    {
        OmChannel& chan1 = ChannelDataWrapper(id1).GetChannel();
        OmChannel& chan2 = ChannelDataWrapper(id2).GetChannel();
        return compareVolumes(&chan1, &chan2);
    }

    static bool CompareSegmentations(const OmID id1, const OmID id2)
    {
        OmSegmentation* seg1 = SegmentationDataWrapper(id1).GetSegmentationPtr();
        OmSegmentation* seg2 = SegmentationDataWrapper(id2).GetSegmentationPtr();
        return compareVolumes(seg1, seg2);
    }

private:
    /*
     * Returns true if two given volumes are exactly the same.
     * Prints all positions where volumes differ if verbose flag is set.
     */
    template <typename VOL>
    static bool compareVolumes(VOL *vol1, VOL *vol2)
    {
        //check if dimensions are the same
        if (vol1->Coords().GetDataExtent().getUnitDimensions() !=
            vol2->Coords().GetDataExtent().getUnitDimensions()){
            printf("Volumes differ: Different dimensions.\n");
            return false;
        }

        //root mip level should be the same if data dimensions are the same
        if( vol1->Coords().GetRootMipLevel() != vol2->Coords().GetRootMipLevel() ){
            printf("Volumes differ: Different number of MIP levels.\n");
            return false;
        }

        for (int level = 0; level <= vol1->Coords().GetRootMipLevel(); ++level) {
            printf("Comparing mip level %i\n",level);

            om::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
                vol1->GetMipChunkCoords(level);

            FOR_EACH(iter, *coordsPtr){
                const OmChunkCoord& coord = *iter;

                if(CompareChunks(coord, vol1, vol2)){
                    continue;
                }

                std::cout << "\tchunks differ at "
                          << coord << "; aborting...\n";
                return false;
            }
        }

        return true;
    }

    /*
     * Returns true if two given chunks contain the exact same image data
     */
    template <typename VOL>
    static bool CompareChunks(const OmChunkCoord& coord,
                              VOL* vol1, VOL* vol2)
    {
        OmChunk* chunk1= vol1->GetChunk(coord);

        OmChunk* chunk2 = vol2->GetChunk(coord);

        if(chunk1->GetCoordinate() != chunk2->GetCoordinate()){
            printf("Chunks differ: Different coords.\n");
            return false;
        }

        if(chunk1->GetDimensions() != chunk2->GetDimensions()){
            printf("Chunks differ: Different dimensions.\n");
            return false;
        }

        return chunk1->Data()->Compare(chunk2->Data());
    }
};

#endif
