#include "mesh/omMeshManagers.hpp"
#include "gui/meshPreviewer/scaleFactorLineEdit.hpp"
#include "gui/meshPreviewer/meshPreviewer.hpp"
#include "gui/meshPreviewer/previewButton.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "view3d/omView3d.h"
#include "chunks/omChunk.h"
#include "volume/omSegmentation.h"
#include "zi/omUtility.h"

#include <boost/make_shared.hpp>

MeshPreviewerImpl::MeshPreviewerImpl(QWidget* parent,
                                     const SegmentationDataWrapper& sdw,
                                     OmViewGroupState* vgs)
    : QWidget(parent)
    , sdw_(sdw)
    , vgs_(vgs)
{
    QLineEdit* downsample = new QLineEdit(this);
    QLineEdit* initialError = new QLineEdit(this);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Initial Error:"), initialError);
    formLayout->addRow(tr("&Edge shrink factor:"), downsample);

    QVBoxLayout* overallContainer = new QVBoxLayout(this);
    overallContainer->addLayout(formLayout);

    OmView3d* v3d = new OmView3d(this, vgs_);
    overallContainer->addWidget(v3d);

    PreviewButton* pb = new PreviewButton(this);
    overallContainer->addWidget(pb);
}

void MeshPreviewerImpl::mesh()
{
//     OmSegmentation& segmentation = sdw_.GetSegmentation();
//     const om::coords::Data coord = om::normCoord(0.5, 0.5, 0.5).toDataCoord();

/*
    segmentation.MeshManagers()->MeshChunk(coord);

    OmChunk* chunk;
    segmentation.GetChunk(chunk, coord);
// select all segments
OmSegmentSelector sel(sdw_, this, "meshPreviewer");
sel.selectNoSegments();
FOR_EACH(iter, chunk->GetUniqueSegIDs()){
sel.augmentSelectedSet(*iter, true);
}
sel.sendEvent();
*/
}
