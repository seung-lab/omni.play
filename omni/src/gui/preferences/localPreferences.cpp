#include "localPreferences.h"
#include "common/omDebug.h"
#include "common/omLocalPreferences.h"
#include "system/omEventManager.h"
#include "system/events/omPreferenceEvent.h"
#include "system/omPreferenceDefinitions.h"

LocalPreferences::LocalPreferences(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox("Settings");
	overallContainer->addWidget( groupBox );

	QGridLayout *grid = new QGridLayout( groupBox );
	grid->addWidget( makeNumberOfThreadsBox(), 0, 0 );
	grid->addWidget( makeCachePropBox(), 1, 0 );
	init_cache_prop_values();

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

QGroupBox* LocalPreferences::makeCachePropBox()
{
	QGroupBox* groupBox = new QGroupBox("Cache Properties");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

        QLabel* ramLabel = new QLabel(groupBox);
        ramLabel->setObjectName(QString("ramLabel"));
        ramLabel->setToolTip("(MB)");
        ramLabel->setText("RAM Cache");
        gridLayout->addWidget(ramLabel, 0, 0, 1, 1);

        ramSlider = new QSlider(groupBox);
        ramSlider->setObjectName(QString("ramSlider"));
        ramSlider->setMinimum(100);
        ramSlider->setMaximum(10000);
        ramSlider->setSingleStep(1);
        ramSlider->setOrientation(Qt::Horizontal);
        ramSlider->setTickPosition(QSlider::TicksBelow);
        gridLayout->addWidget(ramSlider, 0, 1, 1, 1);

        ramSizeLabel = new QLabel(groupBox);
        ramSizeLabel->setObjectName(QString("ramSizeLabel"));
	ramSizeLabel->setText("size");
        gridLayout->addWidget(ramSizeLabel, 1, 1, 1, 1);


        vramSizeLabel = new QLabel(groupBox);
        vramSizeLabel->setObjectName(QString("vramSizeLabel"));
        vramSizeLabel->setText("size");
        gridLayout->addWidget(vramSizeLabel, 3, 1, 1, 1);

	vramSlider = new QSlider(groupBox);
        vramSlider->setObjectName(QString("vramSlider"));
        vramSlider->setMinimum(100);
        vramSlider->setMaximum(10000);
        vramSlider->setSingleStep(1);
        vramSlider->setOrientation(Qt::Horizontal);
        vramSlider->setTickPosition(QSlider::TicksBelow);
        gridLayout->addWidget(vramSlider, 2, 1, 1, 1);

        vramLabel = new QLabel(groupBox);
        vramLabel->setObjectName(QString("vramLabel"));
	vramLabel->setToolTip("(MB)");
	vramLabel->setText("VRAM Cache");
        gridLayout->addWidget(vramLabel, 2, 0, 1, 1);

	return groupBox;
}

void LocalPreferences::init_cache_prop_values()
{
	ramSlider->setValue( OmLocalPreferences::getRamCacheSize());
	ramSizeLabel->setNum( OmLocalPreferences::getRamCacheSize());

	vramSlider->setValue(OmLocalPreferences::getVRamCacheSize());
	vramSizeLabel->setNum(OmLocalPreferences::getVRamCacheSize());
}

void LocalPreferences::on_ramSlider_valueChanged()
{
	OmLocalPreferences::setRamCacheSize( ramSlider->value() );
	ramSizeLabel->setNum(ramSlider->value());
	OmEventManager::PostEvent(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, CACHE_SIZE_PREFERENCES));
}

void LocalPreferences::on_vramSlider_valueChanged()
{
	OmLocalPreferences::setVRamCacheSize( vramSlider->value() );
	vramSizeLabel->setNum(vramSlider->value());
	OmEventManager::PostEvent(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, CACHE_SIZE_PREFERENCES ));
}

QGroupBox* LocalPreferences::makeNumberOfThreadsBox()
{
	QGroupBox* groupBox = makeBoxGeneric( &numThreadsSliderLabel, &numThreadsSlider, "Number of Meshing Threads");

	numThreadsSlider->setMinimum( 1 );
	numThreadsSlider->setMaximum( 2 * OmLocalPreferences::get_num_cores() );

	numThreadsSlider->setValue( OmLocalPreferences::numAllowedWorkerThreads() );
	numThreadsSliderLabel->setNum(numThreadsSlider->value());
	connect(numThreadsSlider, SIGNAL(valueChanged(int)), this, SLOT(on_numThreads_Slider_valueChanged()));

	return groupBox;
}

void LocalPreferences::on_numThreads_Slider_valueChanged()
{
	numThreadsSliderLabel->setNum(numThreadsSlider->value());
	OmLocalPreferences::setNumAllowedWorkerThreads( numThreadsSlider->value() );
}
