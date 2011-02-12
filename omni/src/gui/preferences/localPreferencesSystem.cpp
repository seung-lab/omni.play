#include "localPreferencesSystem.h"
#include "gui/guiUtils.hpp"
#include "common/omDebug.h"
#include "system/omLocalPreferences.hpp"

LocalPreferencesSystem::LocalPreferencesSystem(QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );
	overallContainer->addWidget( makeCachePropBox());
	overallContainer->insertStretch( 4, 1 );
	init_cache_prop_values();

	connect(ramSlider, SIGNAL(valueChanged(int)),
			this, SLOT( on_ramSlider_valueChanged()));
	connect(vramSlider, SIGNAL(valueChanged(int)),
			this, SLOT(on_vramSlider_valueChanged()));

	connect(ramSlider, SIGNAL(sliderReleased()),
			this, SLOT( on_ramSlider_sliderReleased()));
	connect(vramSlider, SIGNAL(sliderReleased()),
			this, SLOT(on_vramSlider_sliderReleased()));
}

QGroupBox* LocalPreferencesSystem::makeCachePropBox()
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

void LocalPreferencesSystem::init_cache_prop_values()
{
	const unsigned int ramSize = OmLocalPreferences::getRamCacheSizeMB();
	ramSlider->setValue( ramSize );
	ramSizeLabel->setText( QString::number( ramSize ));

	const unsigned int vramSize = OmLocalPreferences::getVRamCacheSizeMB();
	vramSlider->setValue( vramSize );
	vramSizeLabel->setText( QString::number( vramSize ) );
}

void LocalPreferencesSystem::on_ramSlider_valueChanged()
{
	ramSizeLabel->setNum(ramSlider->value());
}

void LocalPreferencesSystem::on_vramSlider_valueChanged()
{
	vramSizeLabel->setNum(vramSlider->value());
}

void LocalPreferencesSystem::on_ramSlider_sliderReleased()
{
	ramSizeLabel->setNum(ramSlider->value());
	OmLocalPreferences::setRamCacheSizeMB( ramSlider->value() );
}

void LocalPreferencesSystem::on_vramSlider_sliderReleased()
{
	vramSizeLabel->setNum(vramSlider->value());
	OmLocalPreferences::setVRamCacheSizeMB( vramSlider->value() );
}

