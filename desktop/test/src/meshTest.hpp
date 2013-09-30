#pragma once

#include "mesh/omMeshManagers.hpp"
#include "chunks/omChunkUtils.hpp"
#include "chunks/omSegChunk.h"
#include "common/common.h"
#include "mesh/mesher/TriStripCollector.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "mesh/omMeshParams.hpp"
#include "utility/omLockedPODs.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"

#include "zi/omThreads.h"

class MeshTest {
public:
    MeshTest(OmSegmentation* segmentation, const double threshold)
        : segmentation_(segmentation)
        , threshold_(threshold)
        , meshManager_(segmentation->MeshManager(threshold))
        , meshWriter_(new OmMeshWriterV2(meshManager_))
        , numParallelChunks_(numberParallelChunks())
        , numThreadsPerChunk_(zi::system::cpu_count / 2)
    {}

    ~MeshTest()
    {}

    void MeshFullVolume()
    {
        init();
        meshWriter_->Join();
        meshWriter_->CheckEverythingWasMeshed();
        meshManager_->Metadata()->SetMeshedAndStorageAsChunkFiles();
        check();
    }

private:

    OmSegmentation *const segmentation_;
    const double threshold_;

    OmMeshManager *const meshManager_;
    boost::scoped_ptr<OmMeshWriterV2> meshWriter_;

    const int numParallelChunks_;
    const int numThreadsPerChunk_;

    void init()
    {
        zi::task_manager::simple manager( numParallelChunks_ );
        manager.start();

        om::shared_ptr<std::deque<om::chunkCoord> > coords =
            segmentation_->GetMipChunkCoords(0);

        FOR_EACH(cc, *coords){
            manager.push_back(
                zi::run_fn(
                    zi::bind( &MeshTest::processChunk, this, *cc ) ));
        }

        manager.join();

        std::cout << "\ndone meshing..." << std::endl;
    }

    class MockTriStripCollector : public TriStripCollector {
    public:
        MockTriStripCollector(uint32_t id){
            data_ = std::vector<float>(id, id + 0.1);
            indices_ = std::vector<uint32_t>(id, id + 1);
            strips_ = std::vector<uint32_t>(id, id + 2);
        }
    };

    LockedVector<boost::shared_ptr<MockTriStripCollector> > tris_;

    void processChunk( om::chunkCoord coord )
    {
        FOR_EACH(id,
                 segmentation_->ChunkUniqueValues()->Values(coord,
                                                            threshold_))
        {
            boost::shared_ptr<MockTriStripCollector> t =
                boost::make_shared<MockTriStripCollector>(*id);
            tris_.push_back(t);
            TriStripCollector* pt = t.get();
            meshWriter_->Save(*id, coord, pt,
                              om::common::ShouldBufferWrites::BUFFER_WRITES,
                              om::common::AllowOverwrite::OVERWRITE);
        }
    }


    static int numberParallelChunks()
    {
        // each thread eats a lot of memory (pre loads the data)
        const int megsMemNeededPerChunk = 5000;

        const int sysMemMegs = zi::system::memory::total_mb();

        const int numChunks = sysMemMegs / megsMemNeededPerChunk;

        if(numChunks < 2){
            return 2;
        }

        return numChunks;
    }

    void check(){
        printf("checking mesh data\n");

        auto coords =
            segmentation_->GetMipChunkCoords(0);

        FOR_EACH(cc, *coords){
            FOR_EACH(id,
                     segmentation_->ChunkUniqueValues()->Values(*cc, 1))
            {
                OmMeshPtr mesh;
                segmentation_->MeshManagers()->GetMesh(mesh, *cc, *id, 1,
                                                       om::common::Blocking::BLOCKING);
                if(!mesh){
                    throw om::IoException("no mesh found");
                }

                OmDataForMeshLoad* data = mesh->Data();
                float* vd = data->VertexData();
                uint32_t* vi = data->VertexIndex();
                uint32_t* sd = data->StripData();

                for(int i = 0; i < *id; ++i){
                    float ff = *id + 0.1;
                    if(!qFuzzyCompare(vd[i], ff)){
                        throw om::IoException("bad vertex data");
                    }
                    if(vi[i] != (*id + 1)){
                        throw om::IoException("bad vertex index data");
                    }
                    if(sd[i] != (*id + 2)){
                        throw om::IoException("bad strip data");
                    }
                }
            }
        }

        printf("data check ok!!\n");
    }
};
