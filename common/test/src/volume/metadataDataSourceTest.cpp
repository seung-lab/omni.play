#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "utility/UUID.hpp"

#include "volume/metadataDataSource.hpp"

using namespace om::volume;

namespace om {
namespace test {

#define CHAN_URI "test/data/test.omni.files/channels/channel1"
#define SEG_URI "test/data/test.omni.files/segmentations/segmentation1"

TEST(Volume_MetadataDataSource, GetMetadata) {
  MetadataDataSource source;

  auto chan = source.Get(CHAN_URI);
  auto seg = source.Get(SEG_URI);
  ASSERT_NO_THROW(source.Get("bogus"));

  ASSERT_NE(nullptr, chan.get());
  ASSERT_NE(nullptr, seg.get());
}

TEST(Volume_MetadataDataSource, PutMetadata) {

  MetadataDataSource source;
  auto m1 = source.Get(SEG_URI);
  ASSERT_NE(nullptr, m1);

  auto m2 = std::make_shared<Metadata>();
  m2->Name = "Test Volume";
  m2->UUID = utility::UUID();
  m2->Bounds = coords::GlobalBbox(coords::Global::ZERO, coords::Global(128));
  m2->Resolution = Vector3i(256);
  m2->ChunkDim = 128;
  m2->RootMipLevel = 1;
  m2->DataType = common::DataType::UINT32;
  m2->VolumeType = common::ObjectType::SEGMENTATION;
  m2->NumSegments = 10;
  m2->MaxSegments = 10;

  ASSERT_NO_THROW(source.Put(SEG_URI, m2));
  ASSERT_EQ(*m2, *source.Get(SEG_URI));
}

#undef URI
}
}  // namespace om::test::