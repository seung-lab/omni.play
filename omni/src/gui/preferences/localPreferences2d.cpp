#include <QtGui>
#include "gui/guiUtils.hpp"
#include "gui/preferences/localPreferences2d.h"
#include "common/omDebug.h"
#include "system/omLocalPreferences.h"

LocalPreferences2d::LocalPreferences2d(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	overallContainer->addWidget( makeGeneralPropBox());
	overallContainer->insertStretch(2, 1);
}

QGroupBox* LocalPreferences2d::makeGeneralPropBox()
{
	QGroupBox* groupBox = new QGroupBox("General");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );
/*
	// 2D View Frames in 3D CheckBox
	showCrosshairsCheckbox_ = new QCheckBox(groupBox);
	showCrosshairsCheckbox_->setText("Show Crosshairs");
	const bool viewSquare = Om3dPreferences::get2DViewFrameIn3D();
	viewSquareCheckBox->setChecked(viewSquare);
	gridLayout->addWidget(viewSquareCheckBox, 1, 0, 1, 1);

	connect(discoCheckBox, SIGNAL(stateChanged(int)),
			this, SLOT(on_discoCheckBox_stateChanged()));
*/
  	return groupBox;
}
/*
void LocalPreferences3d::on_viewSquareCheckBox_stateChanged()
{
	const bool val = GuiUtils::getBoolState( viewSquareCheckBox->checkState() );
	Om3dPreferences::set2DViewFrameIn3D( val );
	OmEvents::Redraw3d();
}
*/
