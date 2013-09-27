#pragma once

#include "chunks/omChunkData.hpp"
#include "chunks/omSegChunk.h"
#include "datalayer/hdf5/omHdf5ChunkUtils.hpp"
#include "utility/dataWrappers.h"
#include "volume/io/omVolumeData.h"
#include "volume/omVolumeTypes.hpp"

// extract volumes from hdf5 to mem-map

class OmUpgradeTo14{
public:
    void copyDataOutFromHDF5(){
        doConvert();
    }

private:

    void doConvert()
    {
        FOR_EACH(iter, ChannelDataWrapper::ValidIDs())
        {
            printf("converting channel %d\n", *iter);
            ChannelDataWrapper cdw(*iter);
            convertVolume(cdw.GetChannel());
        }

        FOR_EACH(iter, SegmentationDataWrapper::ValidIDs())
        {
            printf("converting segmentation %d\n", *iter);
            SegmentationDataWrapper sdw(*iter);
            OmSegmentation& seg = sdw.GetSegmentation();
            convertVolume(seg);
            seg.LoadVolData();
        }
    }

    template <typename T>
    void convertVolume(T& vol)
    {
        allocate(vol);
        copyData(vol);
    }

    template <typename T>
    void allocate(T& vol)
    {
        std::map<int, Vector3i> levelsAndDims;

        for (int level = 0; level <= vol.Coords().GetRootMipLevel(); level++) {
            levelsAndDims[level] = vol.Coords().getDimsRoundedToNearestChunk(level);
        }

        // allocate mem-mapped files...
        vol.VolData()->create(&vol, levelsAndDims);

        vol.UpdateFromVolResize();
    }

    template <typename T>
    void copyData(T& vol)
    {
        const uint32_t numChunks = vol.Coords().ComputeTotalNumChunks();
        uint32_t counter = 0;

        for(int level = 0; level <= vol.Coords().GetRootMipLevel(); ++level) 
        {
            if(!OmHdf5ChunkUtils::VolumeExistsInHDF5(&vol, level)){
                printf("no HDF5 volume data found for mip %d\n", level);
                continue;
            }

            std::shared_ptr<std::deque<om::chunkCoord> > coordsPtr =
                vol.GetMipChunkCoords(level);

            FOR_EACH(iter, *coordsPtr)
            {
                ++counter;
                printf("\rcopying chunk %d of %d...", counter, numChunks);
                fflush(stdout);

                copyChunk(vol, *iter);
            }
        }
    }

    template <typename T>
    void copyChunk(T& vol, const om::chunkCoord& coord)
    {
        OmChunk* chunk = vol.GetChunk(coord);

        OmDataWrapperPtr hdf5 =
            OmHdf5ChunkUtils::ReadChunkData(&vol, chunk);

        chunk->Data()->CopyInChunkData(hdf5);
    }
};

