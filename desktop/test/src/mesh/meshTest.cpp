#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "view3d/mesh/omMeshManagers.hpp"
#include "chunks/omChunkUtils.hpp"
#include "common/common.h"
#include "view3d/mesh/mesher/TriStripCollector.hpp"
#include "view3d/mesh/io/omMeshMetadata.hpp"
#include "view3d/mesh/omMeshParams.hpp"
#include "utility/omLockedPODs.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"
#include "volume/OmSimpleRawVol.hpp"

const std::string fnp("../../test_data/build_project/meshTest.omni");

namespace om {
namespace test {

class MockTriStripCollector : public TriStripCollector {
public:
  MockTriStripCollector(uint32_t id) {
    data_ = std::vector<float>(id, id + 0.1);
    indices_ = std::vector<uint32_t>(id, id + 1);
    strips_ = std::vector<uint32_t>(id, id + 2);
  }
};

class MeshTest {
public:
  MeshTest(OmSegmentation* segmentation, const double threshold)
    : segmentation_(segmentation),
      threshold_(threshold),
      meshManager_(segmentation->MeshManager(threshold)),
      meshWriter_(new OmMeshWriterV2(meshManager_)),
      numParallelChunks_(numberParallelChunks()) {
  }

  void MeshFullVolume() {
    init();
    meshWriter_->Join();
    meshWriter_->CheckEverythingWasMeshed();
    meshManager_->Metadata()->SetMeshedAndStorageAsChunkFiles();
    check();
  }

private:
  OmSegmentation* const segmentation_;
  const double threshold_;

  OmMeshManager* meshManager_;
  std::unique_ptr<OmMeshWriterV2> meshWriter_;

  const int numParallelChunks_;

  void init() {
    zi::task_manager::simple manager(numParallelChunks_);
    manager.start();

    auto coords = segmentation_->GetMipChunkCoords(0);

    for (auto& cc : *coords) {
      manager.push_back(
            zi::run_fn(zi::bind(&MeshTest::processChunk, this, cc)));
    }

    manager.join();

    log_infos << "done meshing...";
  }

  om::utility::LockedVector<std::shared_ptr<MockTriStripCollector> > tris_;

  void processChunk(om::coords::Chunk coord) {
    auto segIDs = segmentation_->UniqueValuesDS().Get(coord, threshold_);
    if (!segIDs) {
      log_errors << "Unable to process " << coord << " can't load UniqueValues";
      return;
    }
    for(auto& id: *segIDs) {
      auto t = std::make_shared<MockTriStripCollector>(id);
      tris_.push_back(t);
      TriStripCollector* pt = t.get();
      meshWriter_->Save(id, coord, pt,
                        om::common::ShouldBufferWrites::BUFFER_WRITES,
                        om::common::AllowOverwrite::OVERWRITE);
    }
  }

  static int numberParallelChunks() {
    // each thread eats a lot of memory (pre loads the data)
    const int megsMemNeededPerChunk = 5000;

    const int sysMemMegs = zi::system::memory::total_mb();

    const int numChunks = sysMemMegs / megsMemNeededPerChunk;

    if (numChunks < 2) {
      return 2;
    }

    return numChunks;
  }

  void check() {
    log_infos << "checking mesh data";

    auto coords = segmentation_->GetMipChunkCoords(0);

    for (auto& cc : *coords) {
      auto segIDs = segmentation_->UniqueValuesDS().Get(cc);
      if (!segIDs) {
        log_errors << "Unable to check " << cc << " cannot load UniqueValues";
        continue;
      }
      for (auto& id : *segIDs) {
        OmMeshPtr mesh;
        segmentation_->MeshManagers().GetMesh(mesh, cc, id, 1,
                                              om::common::Blocking::BLOCKING);
        if (!mesh) {
          throw om::IoException("no mesh found");
        }

        OmDataForMeshLoad* data = mesh->Data();
        float* vd = data->VertexData();
        uint32_t* vi = data->VertexIndex();
        uint32_t* sd = data->StripData();

        for (int i = 0; i < id; ++i) {
          float ff = id + 0.1;
          if (!qFuzzyCompare(vd[i], ff)) {
            throw om::IoException("bad vertex data");
          }
          if (vi[i] != (id + 1)) {
            throw om::IoException("bad vertex index data");
          }
          if (sd[i] != (id + 2)) {
            throw om::IoException("bad strip data");
          }
        }
      }
    }

    log_infos << "data check ok!!";
  }
};
/*
TEST(meshTest , CreateProject){
  OmProject::New(QString::fromStdString(fnp));

  const uint64_t SIDE_SIZE = 128;

  // make a segmentation
  SegmentationDataWrapper sdw;
  OmSegmentation& seg = sdw.Create();
  seg.BuildBlankVolume(Vector3i(SIDE_SIZE, SIDE_SIZE, SIDE_SIZE));
  seg.LoadVolData();
  seg.Segments().refreshTree();
  OmProject::Save();

  // fill it (sutpidly)
  auto mip0volFile = OmSimpleRawVol::Open(&seg, 0);
  const uint64_t numVoxels = SIDE_SIZE * SIDE_SIZE * SIDE_SIZE;
  const uint32_t maxSeg = SIDE_SIZE * 3;
  std::vector<uint32_t> data(numVoxels);

  uint32_t c = 1;
  printf("filling data...\n");
  for (uint64_t i = 0; i < numVoxels; ++i) {
    data[i] = c++;
    if (c > maxSeg) {
      c = 1;
    }
  }

  mip0volFile->write(reinterpret_cast<const char*>(&data[0]), 4 * numVoxels);
  mip0volFile->flush();

  OmProject::Close();
}

TEST(meshTest , MeshProject){
  OmProject::Load(QString::fromStdString(fnp));

  SegmentationDataWrapper sdw(1);

  {
    MeshTest mt(sdw.GetSegmentation(), 1.0);
    EXPECT_NO_THROW(mt.MeshFullVolume());
  }

  OmProject::Save();
  OmProject::Close();
}
*/
}
}
