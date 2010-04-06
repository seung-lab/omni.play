#include <QtGui>
#include "guiUtils.h"
#include "localPreferences3d.h"
#include "common/omDebug.h"
#include "system/omLocalPreferences.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"


LocalPreferences3d::LocalPreferences3d(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	overallContainer->addWidget( makeGeneralPropBox());
	overallContainer->insertStretch(4, 3);

	connect(discoCheckBox, SIGNAL(stateChanged(int)), 
		this, SLOT(on_discoCheckBox_stateChanged()));

	connect(crosshairSlider, SIGNAL(valueChanged(int)), 
		this, SLOT(on_crosshairSlider_valueChanged()));
	connect(viewSquareCheckBox, SIGNAL(stateChanged(int)), 
		this, SLOT(on_viewSquareCheckBox_stateChanged()));
	connect(crosshairCheckBox, SIGNAL(stateChanged(int)), 
		this, SLOT(on_crosshairCheckBox_stateChanged()));
}

QGroupBox* LocalPreferences3d::makeGeneralPropBox()
{
	QGroupBox* groupBox = new QGroupBox("General");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

	// 2D View Frames in 3D CheckBox
	viewSquareCheckBox = new QCheckBox(groupBox);
	viewSquareCheckBox->setText("2D View Frames");
	bool viewSquare = OmLocalPreferences::getDefault2DViewFrameIn3D();
	viewSquareCheckBox->setChecked(viewSquare);
        gridLayout->addWidget(viewSquareCheckBox, 1, 0, 1, 1);

	// Draw Crosshairs in 3D CheckBox
	crosshairCheckBox = new QCheckBox(groupBox);
	crosshairCheckBox->setText("3D Crosshairs");
	bool crosshair = OmLocalPreferences::getDefaultDrawCrosshairsIn3D();
	crosshairCheckBox->setChecked(crosshair);
        gridLayout->addWidget(crosshairCheckBox, 2, 0, 1, 1);

	// Size of Crosshair
	crosshairLabel = new QLabel(groupBox);
	crosshairLabel->setText("Crosshair Size: ");
	crosshairLabel->setEnabled(crosshair);
	gridLayout->addWidget(crosshairLabel, 3, 0, 1, 1);

	crosshairValue = new QLabel(groupBox);
	unsigned int value = OmLocalPreferences::getDefaultCrosshairValue();
	crosshairValue->setNum(((float)value)/20.0);
	crosshairValue->setEnabled(crosshair);
	gridLayout->addWidget(crosshairValue, 3, 1, 1, 1);  

	crosshairSlider = new QSlider(groupBox);
	crosshairSlider->setObjectName(QString::fromUtf8("crosshairSlider"));
	crosshairSlider->setMinimum(1);
	crosshairSlider->setMaximum(1000);
	crosshairSlider->setOrientation(Qt::Horizontal);
	crosshairSlider->setTickPosition(QSlider::TicksBelow);
	crosshairSlider->setTickInterval(200);
	crosshairSlider->setValue(value);
	crosshairSlider->setEnabled(crosshair);

	gridLayout->addWidget(crosshairSlider, 4, 0, 1, 3);


        // 3D Disco Ball CheckBox
        discoCheckBox = new QCheckBox(groupBox);
        discoCheckBox->setText("Partial Transparency in 3D");
        bool discoBall = OmLocalPreferences::getDefaultDoDiscoBall();
        discoCheckBox->setChecked(discoBall);
        gridLayout->addWidget(discoCheckBox, 5, 0, 1, 1);


  	return groupBox;
}

void
LocalPreferences3d::on_crosshairSlider_valueChanged()
{
	int value = crosshairSlider->value();
	OmLocalPreferences::setCrosshairValue(value);
	float distance = ((float)value)/20.0;
	crosshairValue->setNum(distance);
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void
LocalPreferences3d::on_viewSquareCheckBox_stateChanged()
{
        const bool val = GuiUtils::getBoolState( viewSquareCheckBox->checkState() );
        OmLocalPreferences::set2DViewFrameIn3D( val );
        OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}


void
LocalPreferences3d::on_discoCheckBox_stateChanged()
{
	const bool val = GuiUtils::getBoolState( discoCheckBox->checkState() );
	OmLocalPreferences::setDoDiscoBall( val );
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}
void
LocalPreferences3d::on_crosshairCheckBox_stateChanged()
{
	const bool val = GuiUtils::getBoolState( crosshairCheckBox->checkState() );
	OmLocalPreferences::setDrawCrosshairsIn3D( val );
	crosshairSlider->setEnabled(val);	
	crosshairValue->setEnabled(val);
	crosshairLabel->setEnabled(val);
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}
