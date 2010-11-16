#include "gui/meshPreviewer/scaleFactorLineEdit.hpp"
#include "gui/meshPreviewer/meshPreviewer.hpp"
#include "gui/meshPreviewer/previewButton.hpp"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "view3d/omView3d.h"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "zi/omUtility.h"

#include <boost/make_shared.hpp>

MeshPreviewerImpl::MeshPreviewerImpl(QWidget* parent,
									 boost::shared_ptr<SegmentationDataWrapper> sdw,
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
	OmSegmentation& segmentation = sdw_->GetSegmentation();
	const DataCoord center =
		segmentation.NormToDataCoord(NormCoord(0.5, 0.5, 0.5));
	const OmMipChunkCoord coord = segmentation.DataToMipCoord(center, 0);

	segmentation.MeshChunk(coord);

	OmMipChunkPtr chunk;
	segmentation.GetChunk(chunk, coord);

	// select all segments
	OmSegmentSelector sel(segmentation.GetID(), this, "meshPreviewer");
	sel.selectNoSegments();
	FOR_EACH(iter, chunk->GetDirectDataValues()){
		sel.augmentSelectedSet(*iter, true);
	}
	sel.sendEvent();
}
