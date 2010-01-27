#include "preferencesMesh.h"

#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omDebug.h"

PreferencesMesh::PreferencesMesh(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox("Mesh Options");
	overallContainer->addWidget( groupBox );

	QGridLayout *grid = new QGridLayout( this );
	groupBox->setLayout( grid );

	grid->addWidget( makeNumberOfThreadsBox(), 0, 0 );
	grid->addWidget( makeDecimationBox(), 1, 0 );
	grid->addWidget( makeSharpnessBox(), 2, 0 );
	grid->addWidget( makeSmoothnessBox(), 3, 0 );
}

QGroupBox* PreferencesMesh::makeNumberOfThreadsBox()
{
	QGroupBox* groupBox = new QGroupBox("Number of Meshing Threads");
	QVBoxLayout* vbox = new QVBoxLayout;
	groupBox->setLayout( vbox );

	return groupBox;
}

QGroupBox* PreferencesMesh::makeSmoothnessBox()
{
	QGroupBox* groupBox = new QGroupBox("Normal Smoothness");
	QVBoxLayout* vbox = new QVBoxLayout;
	groupBox->setLayout( vbox );

	nsSliderLabel = new QLabel(groupBox);
	nsSliderLabel->setMinimumSize(QSize(20, 0));
	nsSliderLabel->setMaximumSize(QSize(20, 16777215));
	vbox->addWidget(nsSliderLabel);

	nsSlider = new QSlider(groupBox);
	nsSlider->setMaximum(100);
	nsSlider->setOrientation(Qt::Horizontal);
	nsSlider->setTickPosition(QSlider::TicksBelow);
	nsSlider->setTickInterval(10);
	vbox->addWidget(nsSlider);

	connect(nsSlider, SIGNAL(valueChanged(int)), this, SLOT(on_nsSlider_valueChanged()));
	nsSlider->setValue(OmPreferences::GetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT));
	nsSliderLabel->setNum(nsSlider->value());

	return groupBox;
}

QGroupBox* PreferencesMesh::makeSharpnessBox()
{
	QGroupBox* groupBox = new QGroupBox("Preserved Angle Sharpness");
	QVBoxLayout* vbox = new QVBoxLayout;
	groupBox->setLayout( vbox );

	pasSliderLabel = new QLabel(groupBox);
	pasSliderLabel->setMinimumSize(QSize(20, 0));
	pasSliderLabel->setMaximumSize(QSize(20, 16777215));
	vbox->addWidget(pasSliderLabel);

	pasSlider = new QSlider(groupBox);
	pasSlider->setMaximum(90);
	pasSlider->setOrientation(Qt::Horizontal);
	pasSlider->setTickPosition(QSlider::TicksBelow);
	pasSlider->setTickInterval(10);
	vbox->addWidget(pasSlider);

	connect(pasSlider, SIGNAL(valueChanged(int)), this, SLOT(on_pasSlider_valueChanged()));
	pasSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT)));
	pasSliderLabel->setNum(pasSlider->value());

	return groupBox;
}
QGroupBox* PreferencesMesh::makeDecimationBox()
{
	QGroupBox* groupBox = new QGroupBox("Target Decimation Percentage");
	QVBoxLayout* vbox = new QVBoxLayout;
	groupBox->setLayout( vbox );

	tdpSliderLabel = new QLabel(groupBox);
	tdpSliderLabel->setMinimumSize(QSize(20, 0));
	tdpSliderLabel->setMaximumSize(QSize(20, 16777215));
	vbox->addWidget(tdpSliderLabel);

	tdpSlider = new QSlider;
	tdpSlider->setMaximum(100);
	tdpSlider->setOrientation(Qt::Horizontal);
	tdpSlider->setTickPosition(QSlider::TicksBelow);
	tdpSlider->setTickInterval(10);
	vbox->addWidget(tdpSlider);

	connect(tdpSlider, SIGNAL(valueChanged(int)), this, SLOT(on_tdpSlider_valueChanged()));
	tdpSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_MESH_REDUCTION_PERCENT_FLT)));
	tdpSliderLabel->setNum(tdpSlider->value());

	return groupBox;
}

void PreferencesMesh::on_tdpSlider_valueChanged()
{
	tdpSliderLabel->setNum(tdpSlider->value());
	OmPreferences::SetFloat(OM_PREF_MESH_REDUCTION_PERCENT_FLT, tdpSlider->value());
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}

void PreferencesMesh::on_pasSlider_valueChanged()
{
	pasSliderLabel->setNum(pasSlider->value());
	OmPreferences::SetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT, pasSlider->value());
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}

void PreferencesMesh::on_nsSlider_valueChanged()
{
	nsSliderLabel->setNum(nsSlider->value());
	OmPreferences::SetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT, nsSlider->value());
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}
