#ifndef OM_MESH_CONVERT_V1_TO_V2_HPP
#define OM_MESH_CONVERT_V1_TO_V2_HPP

#include "mesh/io/v2/omMeshReaderV2.hpp"
#include "mesh/io/omMeshConvertV1toV2Task.hpp"
#include "threads/omThreadPool.hpp"

class OmMeshConvertV1toV2 {
private:
    OmSegmentation* segmentation_;
    const double threshold_;

    om::shared_ptr<OmMeshReaderV1> hdf5Reader_;
    om::shared_ptr<OmMeshReaderV2> meshReader_;
    om::shared_ptr<OmMeshWriterV2> meshWriter_;

    OmThreadPool threadPool_;

public:
    OmMeshConvertV1toV2(OmSegmentation* segmentation)
        : segmentation_(segmentation)
        , threshold_(1)
        , hdf5Reader_(new OmMeshReaderV1(segmentation_))
        , meshReader_(new OmMeshReaderV2(segmentation_, threshold_))
        , meshWriter_(new OmMeshWriterV2(segmentation_, threshold_))
    {}

    ~OmMeshConvertV1toV2(){
        threadPool_.stop();
    }

    void Start()
    {
        threadPool_.start(1);
        om::shared_ptr<OmMeshConvertV1toV2Task> task =
            om::make_shared<OmMeshConvertV1toV2Task>(segmentation_);
        threadPool_.addTaskBack(task);
    }

    om::shared_ptr<OmDataForMeshLoad>
    ReadAndConvert(const OmMeshCoord& meshCoord)
    {
        const OmSegID segID = meshCoord.SegID();
        const OmChunkCoord& coord = meshCoord.Coord();

        if(!meshWriter_->Contains(segID, coord)){
            std::cout << "did not find segID " << segID
                      << " in chunk " << coord << "\n";
            return om::make_shared<OmDataForMeshLoad>();
        }

        if(meshWriter_->WasMeshed(segID, coord)){
            return meshReader_->Read(segID, coord);
        }

        om::shared_ptr<OmDataForMeshLoad> mesh =
            hdf5Reader_->Read(segID, coord);

        meshWriter_->Save(segID, coord, mesh,
                          om::BUFFER_WRITES, om::WRITE_ONCE);

        //std::cout << "migrated mesh " << meshCoord << "\n";

        return mesh;
    }
};

#endif
