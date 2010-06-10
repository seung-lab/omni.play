#include "system/omThreadedCache.cpp"
#include "system/omFetchingThread.cpp"
#include "mesh/omMipMesh.h"
#include "volume/omMipChunk.h"
#include "project/omProject.h"
#include "view2d/omTileCoord.h"
#include "view2d/omTextureID.h"
#include "voxel/omMipSegmentDataCoord.h"
#include "voxel/omMipVoxelation.h"

#include "system/omGenericManager.cpp"
#include "volume/omFilter2d.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

#include "common/omCommon.h"
#include "segment/DynamicTree.cpp"
#include "segment/DynamicTreeContainer.cpp"
#include "system/omGroup.h"

// based on http://www.codeproject.com/KB/cpp/templatesourceorg.aspx

template class OmThreadedCache<OmMipChunkCoord, OmMipChunk>;
template class OmThreadedCache<OmMipMeshCoord, OmMipMesh>;
template class OmThreadedCache<OmTileCoord, OmTextureID>;
template class OmThreadedCache<OmMipSegmentDataCoord, OmMipVoxelation>;
template class OmThreadedCache<OmMipChunkCoord, OmSimpleChunk>;

#if 0
template class OmFetchingThread<OmMipChunkCoord, OmMipChunk>;
template class OmFetchingThread<OmMipMeshCoord, OmMipMesh>;
template class OmFetchingThread<OmTileCoord, OmTextureID>;
template class OmFetchingThread<OmMipSegmentDataCoord, OmMipVoxelation>;
#endif

template class OmGenericManager< OmFilter2d >;
template class OmGenericManager< OmChannel >;
template class OmGenericManager< OmSegmentation >;
template class OmGenericManager< OmManageableObject >;
template class OmGenericManager< OmGroup >;

template class DynamicTree<OmId>;
template class DynamicTreeContainer<OmId>;
