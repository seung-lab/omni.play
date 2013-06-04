#include "gtest/gtest.h"
#include <zi/zargs/zargs.hpp>
#include "project/omProject.h"
#include "datalayer/fs/omFile.hpp"
#include "utility/omUUID.hpp"
#include "utility/dataWrappers.h"
#include "meshTest.hpp"

ZiARG_bool(noTilePrefetch, false, "disable tile prefetcher");
ZiARG_bool(noView3dThrottle, false, "disable View3d throttling");

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    OmProject::Load("/home/mjp/meshwritertest/512.omni");

    SegmentationDataWrapper sdw(1);
    MeshTest mt(sdw.GetSegmentationPtr(), 1.0);
    mt.MeshFullVolume();

    int ret = RUN_ALL_TESTS();
    OmProject::Close();
    return ret;
}
