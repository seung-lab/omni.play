#include "gtest/gtest.h"
#include <zi/zargs/zargs.hpp>
#include "project/omProject.h"
#include "datalayer/fs/omFile.hpp"
#include "utility/omUUID.hpp"
#include "utility/dataWrappers.h"

ZiARG_bool(noTilePrefetch, false, "disable tile prefetcher");
ZiARG_bool(noView3dThrottle, false, "disable View3d throttling");

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	OmProject::New(QString::fromStdString(om::file::tempPath() + "/" + OmUUID().Str()));
	ChannelDataWrapper(1).Create();
	SegmentationDataWrapper(1).Create();
	int ret = RUN_ALL_TESTS();
	OmProject::Close();
	return ret;
}