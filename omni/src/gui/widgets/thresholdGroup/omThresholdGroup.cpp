#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/widgets/thresholdGroup/omThresholdGroup.h"

OmThresholdGroup::OmThresholdGroup(DendToolBar * d, const QString & boxName)
	: QGroupBox(boxName, d)
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
	actUponThresholdChange( mThreshold->text().toFloat() );
	updateGui();
}

void OmThresholdGroup::updateGui()
{
	mDendToolbar->updateGui();
}

void OmThresholdGroup::increaseThresholdButtonWasPressed()
{
	addToThreshold(getThresholdEpsilon());
	updateGui();
}

void OmThresholdGroup::decreaseThresholdButtonWasPressed()
{
	addToThreshold(-getThresholdEpsilon());
	updateGui();
}

void OmThresholdGroup::addToThreshold(const float num)
{
        QString value = mThreshold->text();
        float threshold = value.toFloat();
        threshold += num;

	threshold = filterUserEnteredThreshold(threshold);

        value.setNum(threshold);
        mThreshold->setText(value);

	actUponThresholdChange( threshold );
}
