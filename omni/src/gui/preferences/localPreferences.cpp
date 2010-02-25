#include "localPreferences.h"
#include "common/omDebug.h"
#include "system/omLocalPreferences.h"

LocalPreferences::LocalPreferences(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );




	overallContainer->addWidget( makeNumberOfThreadsBox());
	overallContainer->addWidget( makeCachePropBox());
	overallContainer->insertStretch( 4, 1 );
	init_cache_prop_values();

	connect(numThreadsSlider, SIGNAL(valueChanged(int)), 
		this, SLOT(on_numThreadsSlider_valueChanged()));
	connect(ramSlider, SIGNAL(valueChanged(int)), 
		this, SLOT( on_ramSlider_valueChanged()));
	connect(vramSlider, SIGNAL(valueChanged(int)), 
		this, SLOT(on_vramSlider_valueChanged()));

	connect(numThreadsSlider, SIGNAL(sliderReleased()), 
		this, SLOT(on_numThreadsSlider_sliderReleased()));
	connect(ramSlider, SIGNAL(sliderReleased()), 
		this, SLOT( on_ramSlider_sliderReleased()));
	connect(vramSlider, SIGNAL(sliderReleased()), 
		this, SLOT(on_vramSlider_sliderReleased()));
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
	const unsigned int ramSize = OmLocalPreferences::getRamCacheSize();
	ramSlider->setValue( ramSize );
	ramSizeLabel->setText( QString::number( ramSize ));

	const unsigned int vramSize = OmLocalPreferences::getVRamCacheSize();
	vramSlider->setValue( vramSize );
	vramSizeLabel->setText( QString::number( vramSize ) );
}

void LocalPreferences::on_ramSlider_valueChanged()
{
	ramSizeLabel->setNum(ramSlider->value());
}

void LocalPreferences::on_vramSlider_valueChanged()
{
	vramSizeLabel->setNum(vramSlider->value());
}

void LocalPreferences::on_ramSlider_sliderReleased()
{
	ramSizeLabel->setNum(ramSlider->value());
	OmLocalPreferences::setRamCacheSize( ramSlider->value() );
}

void LocalPreferences::on_vramSlider_sliderReleased()
{
	vramSizeLabel->setNum(vramSlider->value());
	OmLocalPreferences::setVRamCacheSize( vramSlider->value() );
}

QGroupBox* LocalPreferences::makeNumberOfThreadsBox()
{
	QGroupBox* groupBox = new QGroupBox("Number of Meshing Threads");
	QVBoxLayout* vbox = new QVBoxLayout;
	groupBox->setLayout( vbox );

	numThreadsSliderLabel = new QLabel(groupBox);
	numThreadsSliderLabel->setMinimumSize(QSize(20, 0));
	numThreadsSliderLabel->setMaximumSize(QSize(20, 16777215));
	vbox->addWidget(numThreadsSliderLabel);

	numThreadsSlider = new QSlider(groupBox);
	numThreadsSlider->setMaximum(100);
	numThreadsSlider->setOrientation(Qt::Horizontal);
	numThreadsSlider->setTickPosition(QSlider::TicksBelow);
	numThreadsSlider->setTickInterval(10);
	vbox->addWidget(numThreadsSlider);

	numThreadsSlider->setMinimum( 1 );
	numThreadsSlider->setMaximum( 2 * OmLocalPreferences::get_num_cores() );

	numThreadsSlider->setValue( OmLocalPreferences::numAllowedWorkerThreads() );
	numThreadsSliderLabel->setNum(numThreadsSlider->value());

	return groupBox;
}

void LocalPreferences::on_numThreadsSlider_valueChanged()
{
	numThreadsSliderLabel->setNum(numThreadsSlider->value());
}

void LocalPreferences::on_numThreadsSlider_sliderReleased()
{
	numThreadsSliderLabel->setNum(numThreadsSlider->value());
	OmLocalPreferences::setNumAllowedWorkerThreads( numThreadsSlider->value() );
}
