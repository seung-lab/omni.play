#include "gtest/gtest.h"
#include <zi/zargs/zargs.hpp>
#include "project/omProject.h"
#include "datalayer/fs/omFile.hpp"
#include "utility/omUUID.hpp"
#include "utility/dataWrappers.h"
#include "meshTest.hpp"
#include "volume/OmSimpleRawVol.hpp"

ZiARG_bool(noTilePrefetch, false, "disable tile prefetcher");
ZiARG_bool(noView3dThrottle, false, "disable View3d throttling");

static const uint64_t SIDE_SIZE = 768;

void Create(const std::string& fnp) {
  OmProject::New(QString::fromStdString(fnp));

  // make a segmentation
  SegmentationDataWrapper sdw;
  OmSegmentation& seg = sdw.Create();
  seg.BuildBlankVolume(Vector3i(SIDE_SIZE, SIDE_SIZE, SIDE_SIZE));
  seg.LoadVolData();
  seg.Segments()->refreshTree();
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

  printf("writing data...\n");
  mip0volFile->write(reinterpret_cast<const char*>(&data[0]), 4 * numVoxels);
  printf("flushing data...\n");
  mip0volFile->flush();

  OmProject::Close();
}

void TestMesh(const std::string& fnp) {
  OmProject::Load(QString::fromStdString(fnp));

  SegmentationDataWrapper sdw(1);
  MeshTest mt(sdw.GetSegmentationPtr(), 1.0);
  mt.MeshFullVolume();

  OmProject::Save();
  OmProject::Close();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  const std::string fnp("/home/mjp/meshwritertest/test.omni");

  // Create(fnp);
  // TestMesh(fnp);

  int ret = RUN_ALL_TESTS();
  return ret;
}
