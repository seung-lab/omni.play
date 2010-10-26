#include "preferencesMesh.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "common/omDebug.h"

PreferencesMesh::PreferencesMesh(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox("Mesh Options");
	overallContainer->addWidget( groupBox );

	QGridLayout *grid = new QGridLayout( groupBox );

	grid->addWidget( makeDecimationBox(), 1, 0 );
	grid->addWidget( makeSharpnessBox(), 2, 0 );
	grid->addWidget( makeSmoothnessBox(), 3, 0 );
	grid->setRowStretch( 4, 1 );
}

QGroupBox* PreferencesMesh::makeBoxGeneric( QLabel** label, QSlider** slider, QString title )
{
	QGroupBox* groupBox = new QGroupBox(title);
	QVBoxLayout* vbox = new QVBoxLayout;
	groupBox->setLayout( vbox );

	(*label) = new QLabel(groupBox);
	(*label)->setMinimumSize(QSize(20, 0));
	(*label)->setMaximumSize(QSize(20, 16777215));
	vbox->addWidget((*label));

	(*slider) = new QSlider(groupBox);
	(*slider)->setMaximum(100);
	(*slider)->setOrientation(Qt::Horizontal);
	(*slider)->setTickPosition(QSlider::TicksBelow);
	(*slider)->setTickInterval(10);
	vbox->addWidget((*slider));

	return groupBox;
}

QGroupBox* PreferencesMesh::makeSmoothnessBox()
{
	QGroupBox* groupBox = makeBoxGeneric( &nsSliderLabel, &nsSlider, "Normal Smoothness");

	connect(nsSlider, SIGNAL(valueChanged(int)), this, SLOT(on_nsSlider_valueChanged()));
	nsSlider->setValue(OmPreferences::GetInteger(om::PREF_MESH_NUM_SMOOTHING_ITERS_INT));
	nsSliderLabel->setNum(nsSlider->value());

	return groupBox;
}

QGroupBox* PreferencesMesh::makeSharpnessBox()
{
	QGroupBox* groupBox = makeBoxGeneric( &pasSliderLabel, &pasSlider, "Preserved Angle Sharpness");

	connect(pasSlider, SIGNAL(valueChanged(int)), this, SLOT(on_pasSlider_valueChanged()));
	pasSlider->setValue(floor(OmPreferences::GetFloat(om::PREF_MESH_PRESERVED_SHARP_ANGLE_FLT)));
	pasSliderLabel->setNum(pasSlider->value());

	return groupBox;
}
QGroupBox* PreferencesMesh::makeDecimationBox()
{
	QGroupBox* groupBox = makeBoxGeneric( &tdpSliderLabel, &tdpSlider, "Target Decimation Percentage");

	connect(tdpSlider, SIGNAL(valueChanged(int)), this, SLOT(on_tdpSlider_valueChanged()));
	tdpSlider->setValue(floor(OmPreferences::GetFloat(om::PREF_MESH_REDUCTION_PERCENT_FLT)));
	tdpSliderLabel->setNum(tdpSlider->value());

	return groupBox;
}

void PreferencesMesh::on_tdpSlider_valueChanged()
{
	tdpSliderLabel->setNum(tdpSlider->value());
	OmPreferences::SetFloat(om::PREF_MESH_REDUCTION_PERCENT_FLT, tdpSlider->value());
	OmEvents::View3dPreferenceChange();
}

void PreferencesMesh::on_pasSlider_valueChanged()
{
	pasSliderLabel->setNum(pasSlider->value());
	OmPreferences::SetFloat(om::PREF_MESH_PRESERVED_SHARP_ANGLE_FLT, pasSlider->value());
	OmEvents::View3dPreferenceChange();
}

void PreferencesMesh::on_nsSlider_valueChanged()
{
	nsSliderLabel->setNum(nsSlider->value());
	OmPreferences::SetInteger(om::PREF_MESH_NUM_SMOOTHING_ITERS_INT, nsSlider->value());
	OmEvents::View3dPreferenceChange();
}
