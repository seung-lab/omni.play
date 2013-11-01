#pragma once

#include <QtGui>

class LocalPreferences2d : public QWidget {
  Q_OBJECT;
  ;
 public:
  LocalPreferences2d(QWidget* parent);

 private:
  QGroupBox* makeGeneralPropBox();
};
