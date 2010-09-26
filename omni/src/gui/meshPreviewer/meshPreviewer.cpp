#include "segment/omSegmentSelector.h"
#include "gui/meshPreviewer/meshPreviewer.hpp"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "volume/omMipChunk.h"
#include "segment/omSegmentCache.h"
#include "view3d/omView3d.h"

#include <boost/make_shared.hpp>

MeshPreviewerImpl::MeshPreviewerImpl(QWidget* parent,
									 const SegmentationDataWrapper& sdw,
									 OmViewGroupState* vgs)
	: QWidget(parent)
	, sdw_(boost::make_shared<SegmentationDataWrapper>(sdw))
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


}

void MeshPreviewerImpl::mesh()
{
	OmSegmentation& segmentation = sdw_->getSegmentation();
	const DataCoord center =
		segmentation.NormToDataCoord(NormCoord(0.5, 0.5, 0.5));
	const OmMipChunkCoord coord = segmentation.DataToMipCoord(center, 0);

	segmentation.MeshChunk(coord);

	OmMipChunkPtr chunk;
	segmentation.GetChunk(chunk, coord);
	chunk->RefreshDirectDataValues(false, segmentation.GetSegmentCache());

	// select all segments
	OmSegmentSelector sel(segmentation.GetId(), this, "meshPreviewer");
	sel.selectNoSegments();
	FOR_EACH(iter, chunk->GetDirectDataValues()){
		sel.augmentSelectedSet(*iter, true);
	}
	sel.sendEvent();
}
