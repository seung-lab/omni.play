#pragma once

#include "mesh/io/v1/omMeshReaderV1.hpp"
#include "mesh/io/v2/omMeshWriterV2.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "system/cache/omCacheManager.h"
#include "zi/omThreads.h"

class OmMeshConvertV1toV2Task : public zi::runnable{
private:
    OmMeshManager *const meshManager_;
    segmentation *const segmentation_;
    const double threshold_;

    boost::scoped_ptr<OmMeshReaderV1> hdf5Reader_;
    boost::scoped_ptr<OmMeshWriterV2> meshWriter_;

public:
    OmMeshConvertV1toV2Task(OmMeshManager* meshManager)
        : meshManager_(meshManager)
        , segmentation_(meshManager->GetSegmentation())
        , threshold_(meshManager->Threshold())
        , hdf5Reader_(new OmMeshReaderV1(segmentation_))
        , meshWriter_(new OmMeshWriterV2(meshManager))
    {}

    void run()
    {
        printf("copying mesh data...\n");

        om::shared_ptr<std::deque<om::chunkCoord> > coordsPtr =
            segmentation_->GetMipChunkCoords();

        FOR_EACH(iter, *coordsPtr)
        {
            const om::chunkCoord& coord = *iter;

            if(!processChunk(coord)){
                return;
            }
        }

        meshWriter_->Join();

        meshManager_->Metadata()->SetMeshedAndStorageAsChunkFiles();

        printf("mesh conversion done!\n");
    }

private:
    bool processChunk(const om::chunkCoord& coord)
    {
        const ChunkUniqueValues segIDs =
            segmentation_->ChunkUniqueValues()->Values(coord, 1);

        FOR_EACH(segID, segIDs)
        {
            if(OmCacheManager::AmClosingDown())
            {
                // TODO: note that mesh conversion was not done?
                return false;
            }

            if(meshWriter_->WasMeshed(*segID, coord)){
                continue;
            }

            om::shared_ptr<OmDataForMeshLoad> mesh =
                hdf5Reader_->Read(*segID, coord);

            meshWriter_->Save(*segID, coord, mesh,
                              om::DONT_BUFFER_WRITES, om::WRITE_ONCE);
        }

        return true;
    }
};

