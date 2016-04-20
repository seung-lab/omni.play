#include "gtest/gtest.h"
#include "common/logging.h"
#include <zi/zargs/zargs.hpp>

ZiARG_bool(noTilePrefetch, false, "disable tile prefetcher");
ZiARG_bool(noView3dThrottle, false, "disable View3d throttling");
ZiARG_bool(log, false, "Log to console");
ZiARG_bool(verbose, true, "Verbose Log output");
ZiARG_bool(coloredLog, true, "Colored Log output");

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  zi::parse_arguments(argc, argv, true);
  om::logging::initLogging("", true, ZiARG_verbose ? 0 : 120 , ZiARG_coloredLog);
  return RUN_ALL_TESTS();
}
