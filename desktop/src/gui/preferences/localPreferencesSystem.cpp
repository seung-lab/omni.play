#include "system/omConnect.hpp"
#include "localPreferencesSystem.h"
#include "gui/guiUtils.hpp"
#include "common/omDebug.h"
#include "system/omLocalPreferences.hpp"

LocalPreferencesSystem::LocalPreferencesSystem(QWidget* parent)
    : QWidget(parent) {
  QVBoxLayout* overallContainer = new QVBoxLayout(this);
  overallContainer->addWidget(makeCachePropBox());
  overallContainer->insertStretch(4, 1);
  init_cache_prop_values();

  om::connect(meshSlider, SIGNAL(valueChanged(int)), this,
              SLOT(on_meshSlider_valueChanged()));
  om::connect(tileSlider, SIGNAL(valueChanged(int)), this,
              SLOT(on_tileSlider_valueChanged()));

  om::connect(meshSlider, SIGNAL(sliderReleased()), this,
              SLOT(on_meshSlider_sliderReleased()));
  om::connect(tileSlider, SIGNAL(sliderReleased()), this,
              SLOT(on_tileSlider_sliderReleased()));
}

QGroupBox* LocalPreferencesSystem::makeCachePropBox() {
  QGroupBox* groupBox = new QGroupBox("Cache Properties");
  QGridLayout* gridLayout = new QGridLayout;
  groupBox->setLayout(gridLayout);

  QLabel* ramLabel = new QLabel(groupBox);
  ramLabel->setObjectName(QString("ramLabel"));
  ramLabel->setToolTip("(MB)");
  ramLabel->setText("Mesh Cache");
  gridLayout->addWidget(ramLabel, 0, 0, 1, 1);

  meshSlider = new QSlider(groupBox);
  meshSlider->setObjectName(QString("meshSlider"));
  meshSlider->setMinimum(100);
  meshSlider->setMaximum(10000);
  meshSlider->setSingleStep(1);
  meshSlider->setOrientation(Qt::Horizontal);
  meshSlider->setTickPosition(QSlider::TicksBelow);
  gridLayout->addWidget(meshSlider, 0, 1, 1, 1);

  meshSizeLabel = new QLabel(groupBox);
  meshSizeLabel->setObjectName(QString("meshSizeLabel"));
  meshSizeLabel->setText("size");
  gridLayout->addWidget(meshSizeLabel, 1, 1, 1, 1);

  tileSizeLabel = new QLabel(groupBox);
  tileSizeLabel->setObjectName(QString("tileSizeLabel"));
  tileSizeLabel->setText("size");
  gridLayout->addWidget(tileSizeLabel, 3, 1, 1, 1);

  tileSlider = new QSlider(groupBox);
  tileSlider->setObjectName(QString("tileSlider"));
  tileSlider->setMinimum(100);
  tileSlider->setMaximum(10000);
  tileSlider->setSingleStep(1);
  tileSlider->setOrientation(Qt::Horizontal);
  tileSlider->setTickPosition(QSlider::TicksBelow);
  gridLayout->addWidget(tileSlider, 2, 1, 1, 1);

  vramLabel = new QLabel(groupBox);
  vramLabel->setObjectName(QString("vramLabel"));
  vramLabel->setToolTip("(MB)");
  vramLabel->setText("Tile Cache");
  gridLayout->addWidget(vramLabel, 2, 0, 1, 1);

  return groupBox;
}

void LocalPreferencesSystem::init_cache_prop_values() {
  const unsigned int meshSize = OmLocalPreferences::getMeshCacheSizeMB();
  meshSlider->setValue(meshSize);
  meshSizeLabel->setText(QString::number(meshSize));

  const unsigned int tileSize = OmLocalPreferences::getTileCacheSizeMB();
  tileSlider->setValue(tileSize);
  tileSizeLabel->setText(QString::number(tileSize));
}

void LocalPreferencesSystem::on_meshSlider_valueChanged() {
  meshSizeLabel->setNum(meshSlider->value());
}

void LocalPreferencesSystem::on_tileSlider_valueChanged() {
  tileSizeLabel->setNum(tileSlider->value());
}

void LocalPreferencesSystem::on_meshSlider_sliderReleased() {
  meshSizeLabel->setNum(meshSlider->value());
  OmLocalPreferences::setMeshCacheSizeMB(meshSlider->value());
}

void LocalPreferencesSystem::on_tileSlider_sliderReleased() {
  tileSizeLabel->setNum(tileSlider->value());
  OmLocalPreferences::setTileCacheSizeMB(tileSlider->value());
}
