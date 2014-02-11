#include "chunk/cachedDataSource.hpp"
#include "chunk/dataSource.h"
#include "chunk/uniqueValuesFileDataSource.hpp"
#include "common/enums.hpp"
#include "datalayer/cacheDataSource.hpp"
#include "mesh/vertexIndex/dataSource.h"
#include "segment/fileDataSource.hpp"
#include "threads/taskManagerTypes.h"
#include "tile/cachedDataSource.hpp"
#include "tile/dataSource.h"
#include "volume/metadata.hpp"
#include "volume/metadataDataSource.hpp"
#include "volume/metadataManager.h"
#include "volume/volume.h"

namespace om {
namespace volume {

using namespace pipeline;

Volume::Volume(file::path uri)
    : metaDS_(std::make_unique<MetadataDataSource>()),
      metaManager_(std::make_unique<MetadataManager>(*metaDS_, uri)),
      tileDS_(std::make_unique<tile::CachedDataSource>(uri,
                                                       metaManager_->dataType(),
                                                       Coords())),
      chunkDS_(std::make_unique<chunk::CachedDataSource>(
          uri, metaManager_->dataType(), Coords())) {
  log_debugs << "Initialized Volume";
}

Volume::~Volume() {}

tile::TileDS& Volume::TileDS() const { return *tileDS_; }
chunk::ChunkDS& Volume::ChunkDS() const { return *chunkDS_; }

MetadataManager& Volume::Metadata() const { return *metaManager_; }
const coords::VolumeSystem& Volume::Coords() const {
  return metaManager_->coordSystem();
}

void Volume::CloseDownThreads() {}

struct getSegID : public boost::static_visitor<common::SegID> {
  getSegID(uint offset) : offset_(offset) {}

  template <typename T>
  common::SegID operator()(const chunk::Chunk<T>& c) const {
    return c[offset_];
  }

  uint offset_;
};

int Volume::GetBytesPerVoxel() const {
  switch (metaManager_->dataType().index()) {
    case common::DataType::INT8:
    case common::DataType::UINT8: { return 1; }

    case common::DataType::INT32:
    case common::DataType::UINT32:
    case common::DataType::FLOAT: { return 4; }

    case common::DataType::UNKNOWN:
    default: { throw IoException("Unknown datatype."); }
  }
}

const std::string& Volume::GUID() const { return metaManager_->uuid().Str(); }
const std::string& Volume::Endpoint() const { return metaManager_->endpoint(); }
const std::string& Volume::Name() const { return metaManager_->name(); }
}
}  // namespace om::Volume::
