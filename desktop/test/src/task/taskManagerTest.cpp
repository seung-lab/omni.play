#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "task/taskManager.h"

namespace om {
namespace test {

using namespace om::task;

TEST(TaskManagerTest, GetTask) {
  EXPECT_EQ(nullptr, TaskManager::GetTask(0).get());
}
}
}  // namespace om::test::
