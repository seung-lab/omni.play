#include <QtGui> 
#include "preferencesMesh.h" 

#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omDebug.h"


PreferencesMesh::PreferencesMesh(QWidget *parent) 
: QWidget(parent) 
{ 
    this->resize(550, 379);
    QGroupBox* groupBox = new QGroupBox(this);
    groupBox->setTitle("Mesh Options");

    QVBoxLayout* vbox = new QVBoxLayout(groupBox);

    QHBoxLayout* horizontalLayout_3 = new QHBoxLayout();
    QLabel* tdpLabel = new QLabel(groupBox);
    tdpLabel->setText("Target Decimation Percentage:");
    tdpSliderLabel = new QLabel(groupBox);
    tdpSliderLabel->setMinimumSize(QSize(20, 0));
    tdpSliderLabel->setMaximumSize(QSize(20, 16777215));
    horizontalLayout_3->addWidget(tdpLabel);
    horizontalLayout_3->addWidget(tdpSliderLabel);

    vbox->addLayout(horizontalLayout_3);

    tdpSlider = new QSlider(groupBox);
    tdpSlider->setMaximum(100);
    tdpSlider->setOrientation(Qt::Horizontal);
    tdpSlider->setTickPosition(QSlider::TicksBelow);
    tdpSlider->setTickInterval(10);

    gridLayout->addWidget(tdpSlider, 1, 0, 1, 1);

    QHBoxLayout* horizontalLayout_2 = new QHBoxLayout();
    QLabel* pasLabel = new QLabel(groupBox);
    pasLabel->setText("Preserved Angle Sharpness:");

    horizontalLayout_2->addWidget(pasLabel);

    pasSliderLabel = new QLabel(groupBox);
    pasSliderLabel->setObjectName(QString::fromUtf8("pasSliderLabel"));
    pasSliderLabel->setMinimumSize(QSize(20, 0));
    pasSliderLabel->setMaximumSize(QSize(20, 16777215));

    horizontalLayout_2->addWidget(pasSliderLabel);


    gridLayout->addLayout(horizontalLayout_2, 2, 0, 1, 1);

    pasSlider = new QSlider(groupBox);
    pasSlider->setObjectName(QString::fromUtf8("pasSlider"));
    pasSlider->setMaximum(90);
    pasSlider->setOrientation(Qt::Horizontal);
    pasSlider->setTickPosition(QSlider::TicksBelow);
    pasSlider->setTickInterval(10);

    gridLayout->addWidget(pasSlider, 3, 0, 1, 1);

    QHBoxLayout*  horizontalLayout = new QHBoxLayout();
    QLabel* nsLabel = new QLabel(groupBox);
    nsLabel->setText("Normal Smoothness:");

    horizontalLayout->addWidget(nsLabel);

    nsSliderLabel = new QLabel(groupBox);
    nsSliderLabel->setMinimumSize(QSize(20, 0));
    nsSliderLabel->setMaximumSize(QSize(20, 16777215));

    horizontalLayout->addWidget(nsSliderLabel);

    gridLayout->addLayout(horizontalLayout, 4, 0, 1, 1);

    nsSlider = new QSlider(groupBox);
    nsSlider->setMaximum(100);
    nsSlider->setOrientation(Qt::Horizontal);
    nsSlider->setTickPosition(QSlider::TicksBelow);
    nsSlider->setTickInterval(10);

    gridLayout->addWidget(nsSlider, 5, 0, 1, 1);

    tdpLabel->raise();
    tdpSlider->raise();
    pasLabel->raise();
    pasSliderLabel->raise();
    pasSlider->raise();
    nsLabel->raise();
    nsSliderLabel->raise();
    nsSlider->raise();
    tdpSliderLabel->raise();

    gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);


    connect(tdpSlider, SIGNAL(valueChanged(int)), 
		  this, SLOT( on_tdpSlider_valueChanged() ) );
    connect(nsSlider, SIGNAL(valueChanged(int)), 
		  this, SLOT( on_pasSlider_valueChanged() ));
    connect(pasSlider, SIGNAL(valueChanged(int)), 
		  this, SLOT( on_pasSlider_valueChanged() ));
	
	tdpSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_MESH_REDUCTION_PERCENT_FLT)));
	tdpSliderLabel->setNum(tdpSlider->value());
	pasSlider->setValue(floor(OmPreferences::GetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT)));
	pasSliderLabel->setNum(pasSlider->value());
	nsSlider->setValue(OmPreferences::GetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT));
	nsSliderLabel->setNum(nsSlider->value());
}

void PreferencesMesh::on_tdpSlider_valueChanged() {
	tdpSliderLabel->setNum(tdpSlider->value());
	OmPreferences::SetFloat(OM_PREF_MESH_REDUCTION_PERCENT_FLT, tdpSlider->value());
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}

void PreferencesMesh::on_pasSlider_valueChanged() {
	pasSliderLabel->setNum(pasSlider->value());
	OmPreferences::SetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT, pasSlider->value());
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}

void PreferencesMesh::on_nsSlider_valueChanged() {
	nsSliderLabel->setNum(nsSlider->value());
	OmPreferences::SetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT, nsSlider->value());
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}
