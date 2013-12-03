#include "gtest/gtest.h"
#include "common/logging.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  om::logging::initLogging("", false);
  return RUN_ALL_TESTS();
}