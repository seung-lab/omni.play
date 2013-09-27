#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/dataSources.hpp"
#include "datalayer/vector.hpp"

namespace om {
namespace test {

class MockSegmentDataSource : public segment::SegDataDS {
 public:
  MOCK_METHOD2(Get, std::shared_ptr<segment::SegData>(const size_t&, bool));
  MOCK_METHOD3(Put,
               bool(const size_t&, std::shared_ptr<segment::SegData>, bool));
  MOCK_CONST_METHOD0(is_cache, bool());
  MOCK_CONST_METHOD0(is_persisted, bool());
};

// class MockSegDataVector : public segment::SegDataVector {
//   MOCK_METHOD1(Get, segment::Data&(size_t idx));
//   MOCK_CONST_METHOD1(Get, segment::Data&(size_t idx));
//   virtual segment::Data& operator[](size_t idx)override { return Get(idx); }
//   virtual const segment::Data& operator[](size_t idx) const override {
//     return Get(idx);
//   }

//   MOCK_METHOD0(begin, iterator());
//   MOCK_METHOD0(end, iterator());
//   MOCK_CONST_METHOD0(begin, const_iterator());
//   MOCK_CONST_METHOD0(end, const_iterator());
//   MOCK_CONST_METHOD0(cbegin, const_iterator());
//   MOCK_CONST_METHOD0(cend, const_iterator());

//   MOCK_CONST_METHOD0(size, size_t());
//   MOCK_METHOD1(resize, void(size_t n));
//   MOCK_METHOD2(resize, void(size_t n, const segment::Data& val));
//   MOCK_METHOD1(push_back, void(const segment::Data& val));
//   MOCK_METHOD1(push_back, void(segment::Data&& val));
//   MOCK_METHOD0(pop_back, void());
//   MOCK_METHOD0(clear, void());
//   MOCK_METHOD0(flush, void());

//   MOCK_METHOD1(erase, iterator(const_iterator position));
//   MOCK_METHOD2(erase, iterator(const_iterator first, const_iterator last));
// };
}
}  // namespace om::test::
