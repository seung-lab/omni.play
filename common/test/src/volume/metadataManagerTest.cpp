#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "utility/UUID.hpp"
#include "common/enums.hpp"

#include "volume/metadataDataSource.hpp"
#include "volume/metadataManager.h"

using namespace om;
using namespace om::volume;
using namespace om::coords;

namespace om {
namespace test {

#define CHAN_URI "test/data/test.omni.files/channels/channel1"
#define SEG_URI "test/data/test.omni.files/segmentations/segmentation1"

TEST(Volume_MetadataManager, Gets) {
  MetadataDataSource source;
  MetadataManager chanMan(source, CHAN_URI);
  MetadataManager segMan(source, SEG_URI);
  MetadataManager bogus(source, "bogus");

  EXPECT_TRUE(chanMan.HasMetadata());
  EXPECT_TRUE(segMan.HasMetadata());

  EXPECT_NO_THROW(chanMan.name());
  EXPECT_NO_THROW(chanMan.uuid());
  EXPECT_NO_THROW(chanMan.bounds());
  EXPECT_NO_THROW(chanMan.absOffset());
  EXPECT_NO_THROW(chanMan.dimensions());
  EXPECT_NO_THROW(chanMan.resolution());
  EXPECT_NO_THROW(chanMan.chunkDim());
  EXPECT_NO_THROW(chanMan.rootMipLevel());
  EXPECT_NO_THROW(chanMan.dataType());
  EXPECT_NO_THROW(chanMan.volumeType());
  EXPECT_NO_THROW(chanMan.numSegments());
  EXPECT_NO_THROW(chanMan.maxSegments());
  EXPECT_NO_THROW(chanMan.coordSystem());

  EXPECT_NO_THROW(segMan.name());
  EXPECT_NO_THROW(segMan.uuid());
  EXPECT_NO_THROW(segMan.bounds());
  EXPECT_NO_THROW(segMan.absOffset());
  EXPECT_NO_THROW(segMan.dimensions());
  EXPECT_NO_THROW(segMan.resolution());
  EXPECT_NO_THROW(segMan.chunkDim());
  EXPECT_NO_THROW(segMan.rootMipLevel());
  EXPECT_NO_THROW(segMan.dataType());
  EXPECT_NO_THROW(segMan.volumeType());
  EXPECT_NO_THROW(segMan.numSegments());
  EXPECT_NO_THROW(segMan.maxSegments());
  EXPECT_NO_THROW(segMan.coordSystem());

  EXPECT_ANY_THROW(bogus.name());
  EXPECT_ANY_THROW(bogus.uuid());
  EXPECT_ANY_THROW(bogus.bounds());
  EXPECT_ANY_THROW(bogus.absOffset());
  EXPECT_ANY_THROW(bogus.dimensions());
  EXPECT_ANY_THROW(bogus.resolution());
  EXPECT_ANY_THROW(bogus.chunkDim());
  EXPECT_ANY_THROW(bogus.rootMipLevel());
  EXPECT_ANY_THROW(bogus.dataType());
  EXPECT_ANY_THROW(bogus.volumeType());
  EXPECT_ANY_THROW(bogus.numSegments());
  EXPECT_ANY_THROW(bogus.maxSegments());
  EXPECT_ANY_THROW(bogus.coordSystem());
}

TEST(Volume_MetadataManager, Sets) {
  MetadataDataSource source;
  MetadataManager chanMan(source, CHAN_URI);
  MetadataManager segMan(source, SEG_URI);
  MetadataManager bogus(source, "bogus");

  EXPECT_TRUE(chanMan.HasMetadata());
  EXPECT_TRUE(segMan.HasMetadata());

  EXPECT_NO_THROW(chanMan.set_name("chan"));
  EXPECT_EQ("chan", chanMan.name());
  utility::UUID newUUID("00000000-0000-0000-0000-000000000000");
  EXPECT_NO_THROW(chanMan.set_uuid(newUUID));
  EXPECT_EQ(newUUID, chanMan.uuid());

  GlobalBbox bounds(Global::ZERO, Global(128));
  EXPECT_NO_THROW(chanMan.set_bounds(bounds));
  EXPECT_EQ(bounds, chanMan.bounds());

  Global absOffset(1);
  Vector3i dimensions(256);
  EXPECT_NO_THROW(chanMan.set_absOffset(absOffset));
  EXPECT_EQ(absOffset, chanMan.absOffset());
  EXPECT_NO_THROW(chanMan.set_dimensions(dimensions));
  EXPECT_EQ(dimensions, chanMan.dimensions());
  EXPECT_NO_THROW(chanMan.set_resolution(Vector3i(2)));
  EXPECT_EQ(Vector3i(2), chanMan.resolution());
  EXPECT_NO_THROW(chanMan.set_chunkDim(256));
  EXPECT_EQ(256, chanMan.chunkDim());
  EXPECT_NO_THROW(chanMan.set_rootMipLevel(2));
  EXPECT_EQ(2, chanMan.rootMipLevel());
  EXPECT_NO_THROW(chanMan.set_dataType(common::DataType::INT32));
  EXPECT_EQ(common::DataType::INT32, chanMan.dataType().index());
  EXPECT_NO_THROW(chanMan.set_volumeType(common::ObjectType::SEGMENTATION));
  EXPECT_EQ(common::ObjectType::SEGMENTATION, chanMan.volumeType());
  EXPECT_NO_THROW(chanMan.set_numSegments(100));
  EXPECT_EQ(100, chanMan.numSegments());
  EXPECT_NO_THROW(chanMan.set_maxSegments(200));
  EXPECT_EQ(200, chanMan.maxSegments());
  EXPECT_NO_THROW(chanMan.UpdateMaxSegments(220));
  EXPECT_EQ(220, chanMan.maxSegments());
  EXPECT_NO_THROW(chanMan.MaxSegmentsInc());
  EXPECT_EQ(221, chanMan.maxSegments());

  EXPECT_NO_THROW(segMan.set_absOffset(absOffset));
  EXPECT_EQ(absOffset, segMan.absOffset());
  EXPECT_NO_THROW(segMan.set_dimensions(dimensions));
  EXPECT_EQ(dimensions, segMan.dimensions());
  EXPECT_NO_THROW(segMan.set_resolution(Vector3i(2)));
  EXPECT_EQ(Vector3i(2), segMan.resolution());
  EXPECT_NO_THROW(segMan.set_chunkDim(256));
  EXPECT_EQ(256, segMan.chunkDim());
  EXPECT_NO_THROW(segMan.set_rootMipLevel(2));
  EXPECT_EQ(2, segMan.rootMipLevel());
  EXPECT_NO_THROW(segMan.set_dataType(common::DataType::INT32));
  EXPECT_EQ(common::DataType::INT32, segMan.dataType().index());
  EXPECT_NO_THROW(segMan.set_volumeType(common::ObjectType::SEGMENTATION));
  EXPECT_EQ(common::ObjectType::SEGMENTATION, segMan.volumeType());
  EXPECT_NO_THROW(segMan.set_numSegments(100));
  EXPECT_EQ(100, segMan.numSegments());
  EXPECT_NO_THROW(segMan.set_maxSegments(200));
  EXPECT_EQ(200, segMan.maxSegments());
  EXPECT_NO_THROW(segMan.UpdateMaxSegments(220));
  EXPECT_EQ(220, segMan.maxSegments());
  EXPECT_NO_THROW(segMan.MaxSegmentsInc());
  EXPECT_EQ(221, segMan.maxSegments());

  EXPECT_ANY_THROW(bogus.set_absOffset(absOffset));
  EXPECT_ANY_THROW(bogus.set_dimensions(dimensions));
  EXPECT_ANY_THROW(bogus.set_resolution(Vector3i(2)));
  EXPECT_ANY_THROW(bogus.set_chunkDim(256));
  EXPECT_ANY_THROW(bogus.set_rootMipLevel(2));
  EXPECT_ANY_THROW(bogus.set_dataType(common::DataType::INT32));
  EXPECT_ANY_THROW(bogus.set_volumeType(common::ObjectType::SEGMENTATION));
  EXPECT_ANY_THROW(bogus.set_numSegments(100));
  EXPECT_ANY_THROW(bogus.set_maxSegments(200));
  EXPECT_ANY_THROW(bogus.UpdateMaxSegments(220));
  EXPECT_ANY_THROW(bogus.MaxSegmentsInc());
}
}
}