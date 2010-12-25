#include "mesh/io/v2/chunk/omMeshChunkAllocTable.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"

void OmMeshChunkAllocTableV2::registerMappedFile(){
	filePtrCache_->RegisterMappedFile(this);
}
