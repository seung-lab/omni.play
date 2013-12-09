#pragma once

#include "common/common.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "system/omConnect.hpp"

#include <QDialog>
#include <QLabel>
#include <limits>

class DimSpinBox : public OmIntSpinBox {
  float& val_;

 public:
  DimSpinBox(float& val, QWidget* parent)
      : OmIntSpinBox(parent, false), val_(val) {
    setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    setValue(val_);
  }

  void actUponValueChange(const int val) { val_ = val; }
};

class LocationEditDialog : public QDialog {
 private:
  om::globalCoord coord_;

 public:
  LocationEditDialog(const om::globalCoord& coord, QWidget* parent)
      : QDialog(parent), coord_(coord) {
    setModal(true);

    QFormLayout* layout = new QFormLayout;
    layout->addRow(new QLabel("X"), new DimSpinBox(coord_.x, this));
    layout->addRow(new QLabel("Y"), new DimSpinBox(coord_.y, this));
    layout->addRow(new QLabel("Z"), new DimSpinBox(coord_.z, this));
    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    layout->addRow(buttons);

    om::connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    om::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout(layout);
  }

  inline const om::globalCoord& Coord() const { return coord_; }

  static void EditLocation(om::globalCoord& coord, QWidget* parent) {
    LocationEditDialog dialog(coord, parent);
    if (dialog.exec() == QDialog::Accepted) {
      coord = dialog.Coord();
    }
  }
};
