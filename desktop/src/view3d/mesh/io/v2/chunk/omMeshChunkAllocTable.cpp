#include "view3d/mesh/io/v2/chunk/omMeshChunkAllocTable.hpp"
#include "view3d/mesh/io/v2/omMeshFilePtrCache.hpp"

void OmMeshChunkAllocTableV2::registerMappedFile() {
  filePtrCache_->RegisterMappedFile(this);
}
