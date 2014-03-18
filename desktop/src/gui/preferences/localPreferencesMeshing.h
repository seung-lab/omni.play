#pragma once
#include "precomp.h"

class LocalPreferencesMeshing : public QWidget {
  Q_OBJECT;

 public:
  LocalPreferencesMeshing(QWidget* parent);

 private
Q_SLOTS:
  void on_numThreadsSlider_valueChanged();
  void on_numThreadsSlider_sliderReleased();

 private:
  QGroupBox* makeNumberOfThreadsBox();
  QSlider* numThreadsSlider;
  QLabel* numThreadsSliderLabel;

  QGroupBox* makeMeshBox();
};
