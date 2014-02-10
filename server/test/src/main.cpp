#include "gtest/gtest.h"
#include "common/logging.h"
#include <zi/zargs/zargs.hpp>

ZiARG_bool(log, false, "Log to console");

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  zi::parse_arguments(argc, argv, true);
  om::logging::initLogging("", ZiARG_log);
  return RUN_ALL_TESTS();
}