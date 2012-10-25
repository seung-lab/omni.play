#include "gtest/gtest.h"
#include <zi/zargs/zargs.hpp>
#include "project/omProject.h"
#include "datalayer/fs/omFile.hpp"
#include "utility/omUUID.hpp"

ZiARG_bool(noTilePrefetch, false, "disable tile prefetcher");
ZiARG_bool(noView3dThrottle, false, "disable View3d throttling");

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	OmProject::New(QString::fromStdString(om::file::tempPath() + "/" + OmUUID().Str()));
	int ret = RUN_ALL_TESTS();
	OmProject::Close();
	return ret;
}