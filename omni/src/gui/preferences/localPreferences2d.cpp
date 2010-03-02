#include <QtGui>
#include "guiUtils.h"
#include "localPreferences2d.h"
#include "common/omDebug.h"
#include "system/omLocalPreferences.h"

LocalPreferences2d::LocalPreferences2d(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	overallContainer->addWidget( makeGeneralPropBox());
	overallContainer->insertStretch(2, 1);

        connect(stickyCrosshairMode, SIGNAL(stateChanged(int)),
		this, SLOT(on_stickyCrosshairMode_stateChanged(int)));
}

QGroupBox* LocalPreferences2d::makeGeneralPropBox()
{
	QGroupBox* groupBox = new QGroupBox("General");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

        stickyCrosshairMode = new QCheckBox(groupBox);
	stickyCrosshairMode->setText("Sticky Crosshair Mode");
	bool sticky = OmLocalPreferences::getStickyCrosshairMode();
	stickyCrosshairMode->setChecked(sticky);
        gridLayout->addWidget(stickyCrosshairMode, 0, 0, 1, 1);

  	return groupBox;
}

void LocalPreferences2d::on_stickyCrosshairMode_stateChanged( int state)
{
	const bool val = GuiUtils::getBoolState( stickyCrosshairMode->checkState() );
	OmLocalPreferences::setStickyCrosshairMode( val );
}
