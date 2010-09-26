#include "gui/meshPreviewer/meshPreviewer.hpp"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
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

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("Initial Error:"), initialError);
	formLayout->addRow(tr("&Edge shrink factor:"), downsample);

	QVBoxLayout* overallContainer = new QVBoxLayout(this);
	overallContainer->addLayout(formLayout);

	OmView3d* v3d = new OmView3d(this, vgs_);
	overallContainer->addWidget(v3d);


	OmSegmentation & segmentation = sdw_->getSegmentation();

}
