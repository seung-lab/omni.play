#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "chunk/dataSources.hpp"
#include "datalayer/vector.hpp"

namespace om {
namespace test {

class MockUniqueValuesDataSource : public chunk::UniqueValuesDS {
  typedef datalayer::Vector<coords::Chunk, uint32_t> UniqueValues;

 public:
  MOCK_CONST_METHOD2(Get,
                     std::shared_ptr<chunk::UniqueValues>(const coords::Chunk&,
                                                          bool));
  MOCK_METHOD3(Put,
               bool(const coords::Chunk&, std::shared_ptr<UniqueValues>, bool));
  MOCK_CONST_METHOD0(is_cache, bool());
  MOCK_CONST_METHOD0(is_persisted, bool());
};
}
}  // namespace om::test::
