#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/dataSources.hpp"
#include "datalayer/vector.hpp"

namespace om {
namespace test {

class MockSegListDataSource : public segment::SegListDataDS {
 public:
  MOCK_METHOD2(Get, std::shared_ptr<segment::SegListData>(const size_t&, bool));
  MOCK_METHOD3(
      Put, bool(const size_t&, std::shared_ptr<segment::SegListData>, bool));
  MOCK_CONST_METHOD0(is_cache, bool());
  MOCK_CONST_METHOD0(is_persisted, bool());
};
}
}  // namespace om::test::
