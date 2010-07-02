#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/widgets/thresholdGroup/omThresholdGroup.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

static const float thresholdEpsilon = 0.02;

OmThresholdGroup::OmThresholdGroup(DendToolBar * d)
	: QGroupBox("Overall Threshold", d)
	, mDendToolbar(d)
	, thresholdButtonDecrease(new OmThresholdButtonDecrease<OmThresholdGroup>(this,"-",""))
	, thresholdButtonIncrease(new OmThresholdButtonIncrease<OmThresholdGroup>(this,"+",""))
{
	mThreshold = new QLineEdit(this);
	connect(mThreshold, SIGNAL(editingFinished()), 
		this, SLOT(thresholdChanged()));

	QGridLayout* thirdLayout = new QGridLayout(this);
	thirdLayout->addWidget(thresholdButtonDecrease,2,0,1,1);
	thirdLayout->addWidget(mThreshold,1,0,1,2);
	thirdLayout->addWidget(thresholdButtonIncrease,2,1,1,1);
}

void OmThresholdGroup::thresholdChanged()
{
	debug("dendbar", "OmThresholdGroup::thresholdChanged\n");
	haveSegmentationChangeThreshold( mThreshold->text().toFloat() );
	updateGui();
}

void OmThresholdGroup::updateGui()
{
	mDendToolbar->updateGui();
}

void OmThresholdGroup::increaseThresholdButtonWasPressed()
{
	addToThreshold(thresholdEpsilon);
	updateGui();
}

void OmThresholdGroup::decreaseThresholdButtonWasPressed()
{
	addToThreshold(-thresholdEpsilon);
	updateGui();
}

void OmThresholdGroup::addToThreshold(const float num)
{
        QString value = mThreshold->text();
        float threshold = value.toFloat();
        threshold += num;
	if(threshold > 1.0) {
		threshold = 1.0;
	}
	if(threshold < 0.0) {
		threshold = 0.0;
	}
        value.setNum(threshold);
        mThreshold->setText(value);

	haveSegmentationChangeThreshold( threshold );
}
