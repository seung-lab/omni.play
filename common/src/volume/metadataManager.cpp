#include "volume/metadataManager.h"
#include "volume/metadataDataSource.hpp"

namespace om {
namespace volume {

MetadataManager::MetadataManager(MetadataDataSource& mds, file::path uri)
    : mds_(mds), uri_(uri) {
  Update();
}

void MetadataManager::New() { meta_ = std::make_shared<Metadata>(); }

void MetadataManager::Update() {
  zi::guard g(lock_);
  auto meta = mds_.Get(uri_.string());
  if (meta) {
    meta_ = meta;
    log_debugs << "Updated Metadata";
  } else {
    log_debugs << "Unable to get Metadata: " << uri_;
    return;
  }

  coordSystem_.SetDataDimensions(meta_->Bounds.getDimensions() / meta_->Resolution);
  coordSystem_.SetAbsOffset(meta_->Bounds.getMin());
  coordSystem_.SetResolution(meta_->Resolution);
  coordSystem_.SetChunkDimensions(Vector3i(meta_->ChunkDim));
  coordSystem_.UpdateRootLevel();
}

void MetadataManager::UpdateRootLevel() {
  zi::guard g(lock_);
  throwIfInvalid();
  coordSystem_.UpdateRootLevel();
  meta_->RootMipLevel = coordSystem_.RootMipLevel();
}

void MetadataManager::Save() {
  zi::guard g(lock_);
  throwIfInvalid();
  mds_.Put(uri_.string(), meta_);
}
}
}  // namespace om::volume::
