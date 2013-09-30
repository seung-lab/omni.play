#include "system/omConnect.hpp"
#include "gui/guiUtils.hpp"
#include "gui/preferences/localPreferences3d.h"
#include "common/logging.h"
#include "system/omLocalPreferences.hpp"
#include "events/omEvents.h"
#include "view3d/om3dPreferences.hpp"

#include <QtGui>

LocalPreferences3d::LocalPreferences3d(QWidget* parent) : QWidget(parent) {
  QVBoxLayout* overallContainer = new QVBoxLayout(this);

  overallContainer->addWidget(makeGeneralPropBox());
  overallContainer->insertStretch(4, 3);

  om::connect(discoCheckBox, SIGNAL(stateChanged(int)), this,
              SLOT(on_discoCheckBox_stateChanged()));

  om::connect(crosshairSlider, SIGNAL(valueChanged(int)), this,
              SLOT(on_crosshairSlider_valueChanged()));
  om::connect(viewSquareCheckBox, SIGNAL(stateChanged(int)), this,
              SLOT(on_viewSquareCheckBox_stateChanged()));
  om::connect(viewPaneCheckBox, SIGNAL(stateChanged(int)), this,
              SLOT(on_viewPaneCheckBox_stateChanged()));
  om::connect(crosshairCheckBox, SIGNAL(stateChanged(int)), this,
              SLOT(on_crosshairCheckBox_stateChanged()));
}

QGroupBox* LocalPreferences3d::makeGeneralPropBox() {
  QGroupBox* groupBox = new QGroupBox("General");
  QGridLayout* gridLayout = new QGridLayout;
  groupBox->setLayout(gridLayout);

  // 2D View Frames in 3D CheckBox
  viewSquareCheckBox = new QCheckBox(groupBox);
  viewSquareCheckBox->setText("2D View Frames");
  bool viewSquare = Om3dPreferences::get2DViewFrameIn3D();
  viewSquareCheckBox->setChecked(viewSquare);
  gridLayout->addWidget(viewSquareCheckBox, 1, 0, 1, 1);

  viewPaneCheckBox = new QCheckBox(groupBox);
  viewPaneCheckBox->setText("2D Panes in 3D View");
  bool viewPane = Om3dPreferences::get2DViewPaneIn3D();
  viewPaneCheckBox->setChecked(viewPane);
  gridLayout->addWidget(viewPaneCheckBox, 2, 0, 1, 1);

  // Draw Crosshairs in 3D CheckBox
  crosshairCheckBox = new QCheckBox(groupBox);
  crosshairCheckBox->setText("3D Crosshairs");
  bool crosshair = Om3dPreferences::getDrawCrosshairsIn3D();
  crosshairCheckBox->setChecked(crosshair);
  gridLayout->addWidget(crosshairCheckBox, 3, 0, 1, 1);

  // Size of Crosshair
  crosshairLabel = new QLabel(groupBox);
  crosshairLabel->setText("Crosshair Size: ");
  crosshairLabel->setEnabled(crosshair);
  gridLayout->addWidget(crosshairLabel, 4, 0, 1, 1);

  crosshairValue = new QLabel(groupBox);
  unsigned int value = Om3dPreferences::getCrosshairValue();
  crosshairValue->setNum(((float) value) / 20.0);
  crosshairValue->setEnabled(crosshair);
  gridLayout->addWidget(crosshairValue, 4, 1, 1, 1);

  crosshairSlider = new QSlider(groupBox);
  crosshairSlider->setObjectName(QString::fromUtf8("crosshairSlider"));
  crosshairSlider->setMinimum(1);
  crosshairSlider->setMaximum(1000);
  crosshairSlider->setOrientation(Qt::Horizontal);
  crosshairSlider->setTickPosition(QSlider::TicksBelow);
  crosshairSlider->setTickInterval(200);
  crosshairSlider->setValue(value);
  crosshairSlider->setEnabled(crosshair);

  gridLayout->addWidget(crosshairSlider, 5, 0, 1, 3);

  // 3D Disco Ball CheckBox
  discoCheckBox = new QCheckBox(groupBox);
  discoCheckBox->setText("Partial Transparency in 3D (aka \"disco ball\")");
  bool discoBall = Om3dPreferences::getDoDiscoBall();
  discoCheckBox->setChecked(discoBall);
  gridLayout->addWidget(discoCheckBox, 6, 0, 1, 1);

  return groupBox;
}

void LocalPreferences3d::on_crosshairSlider_valueChanged() {
  int value = crosshairSlider->value();
  Om3dPreferences::setCrosshairValue(value);
  float distance = ((float) value) / 20.0;
  crosshairValue->setNum(distance);
  OmEvents::Redraw3d();
}

void LocalPreferences3d::on_viewSquareCheckBox_stateChanged() {
  const bool val = GuiUtils::getBoolState(viewSquareCheckBox->checkState());
  Om3dPreferences::set2DViewFrameIn3D(val);
  OmEvents::Redraw3d();
}

void LocalPreferences3d::on_viewPaneCheckBox_stateChanged() {
  const bool val = GuiUtils::getBoolState(viewPaneCheckBox->checkState());
  Om3dPreferences::set2DViewPaneIn3D(val);
  OmEvents::Redraw3d();
}

void LocalPreferences3d::on_discoCheckBox_stateChanged() {
  const bool val = GuiUtils::getBoolState(discoCheckBox->checkState());
  Om3dPreferences::setDoDiscoBall(val);
  OmEvents::Redraw3d();
}

void LocalPreferences3d::on_crosshairCheckBox_stateChanged() {
  const bool val = GuiUtils::getBoolState(crosshairCheckBox->checkState());
  Om3dPreferences::setDrawCrosshairsIn3D(val);
  crosshairSlider->setEnabled(val);
  crosshairValue->setEnabled(val);
  crosshairLabel->setEnabled(val);
  OmEvents::Redraw3d();
}
