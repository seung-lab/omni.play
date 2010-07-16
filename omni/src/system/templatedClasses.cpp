#include "volume/omThreadChunkLevel.h"
#include "common/omCommon.h"
#include "gui/inspectors/segInspector.h"
#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/validationGroup.h"
#include "gui/toolbars/mainToolbar/mainToolbar.h"
#include "gui/widgets/omButton.cpp"
#include "gui/widgets/omCheckbox.cpp"
#include "mesh/omMipMesh.h"
#include "project/omProject.h"
#include "segment/lowLevel/DynamicTree.cpp"
#include "segment/lowLevel/DynamicTreeContainer.cpp"
#include "segment/lowLevel/omPagingPtrStore.cpp"
#include "system/omGenericManager.cpp"
#include "system/omGroup.h"
#include "system/omHandleCacheMissThreaded.cpp"
#include "system/omThreadedCache.cpp"
#include "view2d/omTextureID.h"
#include "view2d/omTileCoord.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include <QWidget>

// based on http://www.codeproject.com/KB/cpp/templatesourceorg.aspx

template class OmThreadedCache<OmMipChunkCoord, OmMipChunk>;
template class OmThreadedCache<OmMipChunkCoord, OmThreadChunkLevel>;
template class OmThreadedCache<OmMipMeshCoord, OmMipMesh>;
template class OmThreadedCache<OmTileCoord, OmTextureID>;

template class OmGenericManager< OmFilter2d >;
template class OmGenericManager< OmChannel >;
template class OmGenericManager< OmSegmentation >;
template class OmGenericManager< OmManageableObject >;
template class OmGenericManager< OmGroup >;

template class DynamicTree<OmId>;
template class DynamicTreeContainer<OmId>;

template class OmPagingPtrStore<OmSegment>;

template class OmButton<DendToolBar>;
template class OmButton<MainToolbar>;
template class OmButton<MainWindow>;
template class OmButton<QWidget>;
template class OmButton<ValidationGroup>;
template class OmButton<GraphTools>;
template class OmButton<SegInspector>;

template class OmCheckBox<GraphTools>;
