#include "system/omConnect.hpp"
#include "utility/omSystemInformation.h"
#include "localPreferencesMeshing.h"
#include "gui/guiUtils.hpp"
#include "common/logging.h"
#include "system/omLocalPreferences.hpp"

LocalPreferencesMeshing::LocalPreferencesMeshing(QWidget* parent)
    : QWidget(parent) {
  QVBoxLayout* overallContainer = new QVBoxLayout(this);
  overallContainer->addWidget(makeNumberOfThreadsBox());
  overallContainer->addWidget(makeMeshBox());
  overallContainer->insertStretch(4, 1);

  om::connect(numThreadsSlider, SIGNAL(valueChanged(int)), this,
              SLOT(on_numThreadsSlider_valueChanged()));
  om::connect(numThreadsSlider, SIGNAL(sliderReleased()), this,
              SLOT(on_numThreadsSlider_sliderReleased()));
}

QGroupBox* LocalPreferencesMeshing::makeNumberOfThreadsBox() {
  QGroupBox* groupBox = new QGroupBox("Number of Meshing Threads");
  QVBoxLayout* vbox = new QVBoxLayout;
  groupBox->setLayout(vbox);

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

  numThreadsSlider->setMinimum(1);
  numThreadsSlider->setMaximum(2 * OmSystemInformation::get_num_cores());

  numThreadsSlider->setValue(OmLocalPreferences::numAllowedWorkerThreads());
  numThreadsSliderLabel->setNum(numThreadsSlider->value());

  return groupBox;
}

QGroupBox* LocalPreferencesMeshing::makeMeshBox() {
  QGroupBox* groupBox = new QGroupBox("Meshing");
  QGridLayout* gridLayout = new QGridLayout;
  groupBox->setLayout(gridLayout);

  return groupBox;
}

void LocalPreferencesMeshing::on_numThreadsSlider_valueChanged() {
  numThreadsSliderLabel->setNum(numThreadsSlider->value());
}

void LocalPreferencesMeshing::on_numThreadsSlider_sliderReleased() {
  numThreadsSliderLabel->setNum(numThreadsSlider->value());
  OmLocalPreferences::setNumAllowedWorkerThreads(numThreadsSlider->value());
}
