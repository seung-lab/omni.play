#include "localPreferences.h"
#include "common/omDebug.h"
#include "utility/omLocalConfiguration.h"

LocalPreferences::LocalPreferences(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox("Settings");
	overallContainer->addWidget( groupBox );

	QGridLayout *grid = new QGridLayout( groupBox );

	grid->addWidget( makeNumberOfThreadsBox(), 0, 0 );
	grid->setRowStretch( 4, 1 );
}

QGroupBox* LocalPreferences::makeBoxGeneric( QLabel** label, QSlider** slider, QString title )
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


QGroupBox* LocalPreferences::makeNumberOfThreadsBox()
{
	QGroupBox* groupBox = makeBoxGeneric( &numThreadsSliderLabel, &numThreadsSlider, "Number of Meshing Threads");

	numThreadsSlider->setMinimum( 1 );
	numThreadsSlider->setMaximum( 2 * OmLocalConfiguration::get_num_cores() );

	numThreadsSlider->setValue( OmLocalConfiguration::numAllowedWorkerThreads() );
	numThreadsSliderLabel->setNum(numThreadsSlider->value());
	connect(numThreadsSlider, SIGNAL(valueChanged(int)), this, SLOT(on_numThreads_Slider_valueChanged()));

	return groupBox;
}

void LocalPreferences::on_numThreads_Slider_valueChanged()
{
	numThreadsSliderLabel->setNum(numThreadsSlider->value());
	//OmPreferences::SetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT, numThreadsSlider->value());
}
