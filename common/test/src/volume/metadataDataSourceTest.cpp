#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "utility/UUID.hpp"
#include "datalayer/paths.hpp"

#include "volume/metadataDataSource.hpp"

using namespace om::volume;

namespace om {
namespace test {

#define URI \
  "/omniweb_data/x06/y59/x06y59z28_s1587_13491_6483_e1842_13746_6738.omni"
//TODO: the PutMetadata test below modifies the file on disk!

TEST(Volume_MetadataDataSource, GetMetadata) {
  file::Paths p(URI);
  MetadataDataSource source;

  auto chan = source.Get(p.Channel(1).string());
  auto seg = source.Get(p.Segmentation(1).string());
  ASSERT_NO_THROW(source.Get("bogus"));

  ASSERT_NE(nullptr, chan.get());
  ASSERT_NE(nullptr, seg.get());
}

TEST(Volume_MetadataDataSource, PutMetadata) {
  file::Paths p(URI);

  MetadataDataSource source;
  auto m1 = source.Get(p.Segmentation(1).string());
  ASSERT_NE(nullptr, m1);

  auto m2 = std::make_shared<Metadata>();
  m2->Name = "Test Volume";
  m2->UUID = utility::UUID();
  m2->AbsOffset = Vector3i::ZERO;
  m2->DataDimensions = Vector3i(128);
  m2->Resolution = Vector3i(256);
  m2->ChunkDim = 128;
  m2->RootMipLevel = 1;
  m2->DataType = common::DataType::UINT32;
  m2->VolumeType = common::ObjectType::SEGMENTATION;
  m2->NumSegments = 10;
  m2->MaxSegments = 10;

  ASSERT_NO_THROW(source.Put(p.Segmentation(1).string(), m2));
  ASSERT_EQ(*m2, *source.Get(p.Segmentation(1).string()));
}

#undef URI
}
}  // namespace om::test::