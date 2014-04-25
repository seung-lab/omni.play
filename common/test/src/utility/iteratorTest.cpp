#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utility/iterators.hpp"
#include "utility/timer.hpp"
#include "volume/segmentation.h"

namespace om {
namespace test {

using namespace utility;

TEST(Utility_Iterators, VectorIterator) {
  size_t count = 0;

  for (auto iter = vector3_iterator<int>({1, 2, 3}, {2, 4, 6});
       iter != vector3_iterator<int>(); ++iter) {
    count++;
    if (count > 30) {
      break;
    }
  }
  ASSERT_EQ(24, count);
  Vector3i val(1, 4, 3);
  ASSERT_NE(vector3_iterator<int>(),
            std::find(vector3_iterator<int>({1, 2, 3}, {2, 4, 6}),
                      vector3_iterator<int>(), val));

  vector3_iterator<int> a({1, 2, 3}, {1, 2, 3});
  vector3_iterator<int> b;
  ASSERT_NE(b, a);
  ASSERT_EQ(b, ++a);
  ASSERT_NE(vector3_iterator<int>({0, 0, 0}, {10, 11, 12}),
            vector3_iterator<int>());
}

TEST(Utility_Iterators, Benchmark_VectorIterator) {
  const size_t LIMIT = 500;
  utility::timer t;
  t.start();
  size_t inc = 0;
  for (int i = 1; i <= LIMIT; ++i) {
    for (int j = 1; j <= LIMIT; ++j) {
      for (int k = 1; k <= LIMIT; ++k) {
        if ((j + k) % i == 0) inc++;
      }
    }
  }
  t.Print("Standard For loop");

  t.reset();
  t.start();
  size_t inc2 = 0;
  for (vector3_iterator<int> iter({1, 1, 1}, {LIMIT, LIMIT, LIMIT});
       iter != vector3_iterator<int>(); ++iter) {
    auto ref = *iter;
    if ((ref.z + ref.y) % ref.x == 0) inc2++;
  }
  t.Print("vector3_iterator");

  ASSERT_EQ(inc, inc2);
}

TEST(Utility_Iterators, ChunkIterator) {
  size_t count = 0;

  for (auto iter = make_chunk_iterator(coords::Chunk(1, 1, 2, 3),
                                       coords::Chunk(1, 2, 4, 6));
       iter != chunk_iterator(); ++iter) {
    count++;
    if (count > 30) {
      break;
    }
  }
  ASSERT_EQ(24, count);
  coords::Chunk val(1, 1, 4, 3);
  ASSERT_NE(chunk_iterator(),
            std::find(make_chunk_iterator(coords::Chunk(1, 1, 2, 3),
                                          coords::Chunk(1, 2, 4, 6)),
                      chunk_iterator(), val));
}

TEST(Utility_Iterators, Benchmark_ChunkIterator) {
  const size_t LIMIT = 200;
  coords::VolumeSystem vs(Vector3i{LIMIT, LIMIT, LIMIT});

  utility::timer t;
  t.start();
  size_t val = 0;
  for (int i = 0; i <= LIMIT; ++i) {
    for (int j = 0; j <= LIMIT; ++j) {
      for (int k = 0; k <= LIMIT; ++k) {
        coords::Chunk c(0, i, j, k);
        val = (val + c.PtrOffset(vs, 32)) % LIMIT;
      }
    }
  }
  t.Print("Standard For loop");

  t.reset();
  t.start();
  size_t val2 = 0;
  for (auto iter = make_chunk_iterator(coords::Chunk(0, 0, 0, 0),
                                       coords::Chunk(0, LIMIT, LIMIT, LIMIT));
       iter != chunk_iterator(); ++iter) {
    val2 = (val2 + iter->PtrOffset(vs, 32)) % LIMIT;
  }
  t.Print("chunk_iterator");

  ASSERT_EQ(val, val2);
}

TEST(Utility_Iterators, Benchmark_FilteredChunkIterator) {
  volume::Segmentation seg(
      file::Paths("/omniData/e2198/e2198_a_s10_101_46_e17_116_61.omni"), 1);

  const common::SegID id = 37;
  auto chunkDims = seg.Coords().MipLevelDimensionsInMipChunks(0);

  // prime cache.
  for (int i = 0; i <= chunkDims.x; ++i) {
    for (int j = 0; j <= chunkDims.y; ++j) {
      for (int k = 0; k <= chunkDims.z; ++k) {
        auto uv = seg.UniqueValuesDS().Get(coords::Chunk(0, i, j, k));
      }
    }
  }

  utility::timer t;
  t.start();
  size_t count;

  for (int i = 0; i <= chunkDims.x; ++i) {
    for (int j = 0; j <= chunkDims.y; ++j) {
      for (int k = 0; k <= chunkDims.z; ++k) {
        coords::Chunk c(0, i, j, k);
        auto uv = seg.UniqueValuesDS().Get(c);
        if (uv && uv->contains(id)) {
          count++;
        }
      }
    }
  }
  t.Print("Standard For loop");

  t.reset();
  t.start();
  size_t count2 = 0;
  for (auto iter = make_segment_chunk_iterator(coords::Chunk(0, 0, 0, 0),
                                               coords::Chunk(0, chunkDims),
                                               seg.UniqueValuesDS(), id);
       iter != filtered_chunk_iterator(); ++iter) {
    count2++;
  }
  t.Print("chunk_iterator");

  ASSERT_EQ(count, count2);
}

TEST(Utility_Iterators, Benchmark_DatavalIterator) {
  volume::Segmentation seg(
      file::Paths("/omniData/e2198/e2198_a_s10_101_46_e17_116_61.omni"), 1);

  coords::DataBbox bounds({{1, 2, 3}, {500, 300, 400}}, seg.Coords(), 1);

  utility::timer t;
  t.start();
  auto chunkFrom = bounds.getMin().ToChunk();
  auto chunkTo = bounds.getMax().ToChunk();

  size_t sum = 0;
  for (int i = chunkFrom.x; i <= chunkTo.x; ++i) {
    for (int j = chunkFrom.y; j <= chunkTo.y; ++j) {
      for (int k = chunkFrom.z; k <= chunkTo.z; ++k) {
        coords::Chunk c(chunkFrom.mipLevel(), i, j, k);
        auto chunk =
            boost::get<chunk::Chunk<common::SegID>>(seg.ChunkDS().Get(c).get());
        if (!chunk) {
          continue;
        }

        auto chunkBounds = c.BoundingBox(seg.Coords());
        chunkBounds.intersect(bounds);
        auto dataFrom = chunkBounds.getMin();
        auto dataTo = chunkBounds.getMax();

        size_t idx = 0;
        coords::Data d = dataFrom;
        for (d.z = dataFrom.z; d.z < dataTo.z; ++d.z) {
          for (d.y = dataFrom.y; d.y < dataTo.y; ++d.y) {
            d.x = dataFrom.x;
            idx = d.ToChunkOffset();
            for (; d.x < dataTo.x; ++d.x) {
              sum += (*chunk)[idx];
              idx++;
            }
          }
        }
      }
    }
  }
  t.Print("Standard For loop");

  t.reset();
  t.start();
  size_t sum2 = 0;
  for (auto iter =
           make_all_dataval_iterator<common::SegID>(bounds, seg.ChunkDS());
       iter != all_dataval_iterator<common::SegID>(); ++iter) {
    sum2 += iter->second;
  }
  t.Print("dataval_iterator");

  ASSERT_EQ(sum, sum2);
}
}
}  // namespace om::test::