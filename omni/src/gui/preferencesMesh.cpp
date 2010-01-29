#include "preferencesMesh.h"

#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omDebug.h"
#include "system/omNumCores.h"

PreferencesMesh::PreferencesMesh(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox("Mesh Options");
	overallContainer->addWidget( groupBox );

	QGridLayout *grid = new QGridLayout( this );

	grid->addWidget( makeNumberOfThreadsBox(), 0, 0 );
	grid->addWidget( makeDecimationBox(), 1, 0 );
	grid->addWidget( makeSharpnessBox(), 2, 0 );
	grid->addWidget( makeSmoothnessBox(), 3, 0 );
}

QGroupBox* PreferencesMesh::makeNumberOfThreadsBox()
{
	QGroupBox* groupBox = makeBoxGeneric( &numThreadsSliderLabel, &numThreadsSlider, "Number of Meshing Threads");

	numThreadsSlider->setMaximum( 2 * (int)OmNumCores::get_num_cores() );

	connect(numThreadsSlider, SIGNAL(valueChanged(int)), this, SLOT(on_numThreads_Slider_valueChanged()));
	numThreadsSlider->setValue(OmPreferences::GetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT));
	numThreadsSliderLabel->setNum(numThreadsSlider->value());

	return groupBox;
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
	nsSlider->setValue(OmPreferences::GetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT));
	nsSliderLabel->setNum(nsSlider->value());

	return groupBox;
}

QGroupBox* PreferencesMesh::makeSharpnessBox()
{
	QGroupBox* groupBox = makeBoxGeneric( &pasSliderLabel, &pasSlider, "Preserved Angle Sharpness");

	connect(pasSlider, SIGNAL(valueChanged(int)), this, SLOT(on_pasSlider_valueChanged()));
	pasSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT)));
	pasSliderLabel->setNum(pasSlider->value());

	return groupBox;
}
QGroupBox* PreferencesMesh::makeDecimationBox()
{
	QGroupBox* groupBox = makeBoxGeneric( &tdpSliderLabel, &tdpSlider, "Target Decimation Percentage");

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

void PreferencesMesh::on_numThreads_Slider_valueChanged()
{
	numThreadsSliderLabel->setNum(numThreadsSlider->value());
	OmPreferences::SetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT, numThreadsSlider->value());
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}
