#pragma once
#include "precomp.h"

class LocalPreferencesSystem : public QWidget {
  Q_OBJECT;

 public:
  LocalPreferencesSystem(QWidget* parent);

 private
Q_SLOTS:
  void on_shouldJumpCheckBox_stateChanged();
  void on_shouldReturnOldToolCheckBox_stateChanged();

  void on_meshSlider_valueChanged();
  void on_tileSlider_valueChanged();

  void on_meshSlider_sliderReleased();
  void on_tileSlider_sliderReleased();

 private:
  QCheckBox* shouldJumpCheckBox;
  QCheckBox* shouldReturnOldToolCheckBox;
  QLabel* sizeLabel;
  QLabel* meshSizeLabel;
  QLabel* vramLabel;
  QLabel* tileSizeLabel;
  QSlider* meshSlider;
  QSlider* tileSlider;

  QGroupBox* makeCachePropBox();
  QGroupBox* makeActionPropBox();
  void init_cache_prop_values();
};
