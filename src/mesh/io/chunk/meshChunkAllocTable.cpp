#include "mesh/iochunk/meshChunkAllocTable.hpp"
#include "mesh/iomeshFilePtrCache.hpp"

void meshChunkAllocTableV2::registerMappedFile(){
    filePtrCache_->RegisterMappedFile(this);
}
