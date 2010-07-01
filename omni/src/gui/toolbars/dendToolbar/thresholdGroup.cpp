#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "common/omDebug.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "system/omEvents.h"

static const float thresholdEpsilon = 0.02;

ThresholdGroup::ThresholdGroup(DendToolBar * d)
	: QGroupBox("Overall Threshold", d)
	, mDendToolbar(d)
{
	decreaseThresholdAct = new QPushButton(this);
	decreaseThresholdAct->setText("-");
	connect(decreaseThresholdAct, SIGNAL(pressed()), 
		this, SLOT(decreaseThreshold()));


	mThreshold = new QLineEdit(this);
	setThresholdValue();
	connect(mThreshold, SIGNAL(editingFinished()), 
		this, SLOT(thresholdChanged()));

	increaseThresholdAct = new QPushButton(this);
	increaseThresholdAct->setText("+");	

	connect(increaseThresholdAct, SIGNAL(pressed()), 
		this, SLOT(increaseThreshold()));
	
	QGridLayout* thirdLayout = new QGridLayout(this);
	thirdLayout->addWidget(decreaseThresholdAct,2,0,1,1);
	thirdLayout->addWidget(mThreshold,1,0,1,2);
	thirdLayout->addWidget(increaseThresholdAct,2,1,1,1);
}

void ThresholdGroup::thresholdChanged()
{
	debug("dendbar", "ThresholdGroup::thresholdChanged\n");

	haveSegmentationChangeThreshold( mThreshold->text().toFloat() );

	updateGui();
}

void ThresholdGroup::haveSegmentationChangeThreshold( const float threshold )
{
	SegmentationDataWrapper sdw = mDendToolbar->getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}
	
	sdw.getSegmentation().SetDendThresholdAndReload(threshold);
	OmEvents::SegmentModified();
}

void ThresholdGroup::updateGui()
{
	mDendToolbar->updateGui();
}



void ThresholdGroup::increaseThreshold()
{
        debug("dendbar", "DendToolBar::increaseThreshold\n");
	addToThreshold(thresholdEpsilon);

	updateGui();
}

void ThresholdGroup::decreaseThreshold()
{
	debug("dendbar", "DendToolBar::decreaseThreshold\n");
	addToThreshold(-thresholdEpsilon);

	updateGui();
}

void ThresholdGroup::addToThreshold(const float num)
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

void ThresholdGroup::setThresholdValue()
{
	QString value;

	SegmentationDataWrapper sdw = mDendToolbar->getSegmentationDataWrapper();
	if(sdw.isValid()){
		value.setNum( sdw.getSegmentation().GetDendThreshold() );
	} else {
		value.setNum(0.95);
	}
	mThreshold->setText(value);
}
