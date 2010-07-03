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
#include "segment/lowLevel/DynamicTree.cpp"
#include "segment/lowLevel/DynamicTreeContainer.cpp"
#include "system/omGroup.h"

#include "segment/lowLevel/omPagingStore.cpp"

#include "gui/widgets/omButton.cpp"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/mainToolbar/mainToolbar.h"
#include "gui/mainwindow.h"
#include <QWidget>
#include "gui/toolbars/dendToolbar/validationGroup.h"

#include "gui/widgets/omCheckbox.cpp"

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

template class OmPagingStore<OmSegment>;

template class OmButton<DendToolBar>;
template class OmButton<MainToolbar>;
template class OmButton<MainWindow>;
template class OmButton<QWidget>;
template class OmButton<ValidationGroup>;

template class OmCheckBox<DendToolBar>;
