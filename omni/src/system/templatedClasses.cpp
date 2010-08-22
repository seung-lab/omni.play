#include "datalayer/fs/omMemMappedVolume.cpp"
#include "datalayer/hdf5/omHDF5Volume.cpp"
#include "volume/build/omLoadImage.cpp"
#include "volume/build/omVolumeImporter.cpp"
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
#include "system/cache/omHandleCacheMissThreaded.cpp"
#include "system/cache/omThreadedCache.cpp"
#include "view2d/omTextureID.h"
#include "view2d/omTileCoord.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include <QWidget>

#include "datalayer/fs/omActionLoggerFSthread.cpp"
#include "segment/actions/segment/omSegmentGroupAction.h"
#include "segment/actions/segment/omSegmentJoinAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/segment/omSegmentSplitAction.h"
#include "segment/actions/segment/omSegmentValidateAction.h"
#include "volume/omSegmentationThresholdChangeAction.h"
#include "volume/omVoxelSetValueAction.h"

// based on http://www.codeproject.com/KB/cpp/templatesourceorg.aspx

template class OmThreadedCache<OmMipChunkCoord, OmMipChunkPtr>;
template class OmThreadedCache<OmMipChunkCoord, OmThreadChunkLevelPtr>;
template class OmThreadedCache<OmMipMeshCoord, OmMipMeshPtr>;
template class OmThreadedCache<OmTileCoord, OmTextureIDPtr>;

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

template class OmVolumeImporter<OmChannel>;
template class OmVolumeImporter<OmSegmentation>;
template class OmLoadImage<OmChannel>;
template class OmLoadImage<OmSegmentation>;

template class OmMemMappedVolume<int8_t, OmChannel>;
template class OmMemMappedVolume<uint8_t, OmChannel>;
template class OmMemMappedVolume<int32_t, OmChannel>;
template class OmMemMappedVolume<uint32_t, OmChannel>;
template class OmMemMappedVolume<float, OmChannel>;
template class OmMemMappedVolume<int8_t, OmSegmentation>;
template class OmMemMappedVolume<uint8_t, OmSegmentation>;
template class OmMemMappedVolume<int32_t, OmSegmentation>;
template class OmMemMappedVolume<uint32_t, OmSegmentation>;
template class OmMemMappedVolume<float, OmSegmentation>;

template class OmHDF5Volume<int8_t, OmChannel>;
template class OmHDF5Volume<uint8_t, OmChannel>;
template class OmHDF5Volume<int32_t, OmChannel>;
template class OmHDF5Volume<uint32_t, OmChannel>;
template class OmHDF5Volume<float, OmChannel>;
template class OmHDF5Volume<int8_t, OmSegmentation>;
template class OmHDF5Volume<uint8_t, OmSegmentation>;
template class OmHDF5Volume<int32_t, OmSegmentation>;
template class OmHDF5Volume<uint32_t, OmSegmentation>;
template class OmHDF5Volume<float, OmSegmentation>;

template class OmActionLoggerFSThread<OmProjectSaveAction>;
template class OmActionLoggerFSThread<OmSegmentJoinAction>;
template class OmActionLoggerFSThread<OmSegmentValidateAction>;
template class OmActionLoggerFSThread<OmSegmentGroupAction>;
template class OmActionLoggerFSThread<OmSegmentSelectAction>;
template class OmActionLoggerFSThread<OmSegmentSplitAction>;
template class OmActionLoggerFSThread<OmVoxelSetValueAction>;
template class OmActionLoggerFSThread<OmSegmentationThresholdChangeAction>;
