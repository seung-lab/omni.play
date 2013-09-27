#pragma once

#include "mesh/io/v2/omMeshReaderV2.hpp"
#include "mesh/io/omMeshConvertV1toV2Task.hpp"
#include "threads/omTaskManager.hpp"

class OmMeshConvertV1toV2 {
private:
    OmMeshManager *const meshManager_;
    OmSegmentation *const segmentation_;
    const double threshold_;

    std::unique_ptr<OmMeshReaderV1> hdf5Reader_;
    std::unique_ptr<OmMeshReaderV2> meshReader_;
    std::unique_ptr<OmMeshWriterV2> meshWriter_;

    OmThreadPool threadPool_;

public:
    OmMeshConvertV1toV2(OmMeshManager* meshManager)
        : meshManager_(meshManager)
        , segmentation_(meshManager_->GetSegmentation())
        , threshold_(1)
        , hdf5Reader_(new OmMeshReaderV1(segmentation_))
        , meshReader_(new OmMeshReaderV2(meshManager_))
        , meshWriter_(new OmMeshWriterV2(meshManager_))
    {}

    ~OmMeshConvertV1toV2(){
        threadPool_.stop();
    }

    void Start()
    {
        threadPool_.start(1);

        std::shared_ptr<OmMeshConvertV1toV2Task> task =
            std::make_shared<OmMeshConvertV1toV2Task>(meshManager_);

        threadPool_.push_back(task);
    }

    // migrate mesh
    std::shared_ptr<OmDataForMeshLoad>
    ReadAndConvert(const OmMeshCoord& meshCoord)
    {
        const om::common::SegID segID = meshCoord.SegID();
        const om::chunkCoord& coord = meshCoord.Coord();

        if(!meshWriter_->Contains(segID, coord))
        {
            std::cout << "did not find segID " << segID
                      << " in chunk " << coord << "\n";
            return std::make_shared<OmDataForMeshLoad>();
        }

        if(meshWriter_->WasMeshed(segID, coord)){
            return meshReader_->Read(segID, coord);
        }

        std::shared_ptr<OmDataForMeshLoad> mesh =
            hdf5Reader_->Read(segID, coord);

        meshWriter_->Save(segID, coord, mesh,
                          om::common::ShouldBufferWrites::BUFFER_WRITES, om::WRITE_ONCE);

        return mesh;
    }
};

