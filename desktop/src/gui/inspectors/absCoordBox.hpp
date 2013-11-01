#pragma once

#include "system/omConnect.hpp"
#include "project/omProject.h"
#include "volume/omMipVolume.h"

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

namespace om {
namespace volumeInspector {

class AbsCoordBox : public QGroupBox {
  Q_OBJECT;
  ;
 private:
  OmMipVolume& vol_;
  QGridLayout* grid_;
  QLineEdit* absOffsetX_;
  QLineEdit* absOffsetY_;
  QLineEdit* absOffsetZ_;

 private
Q_SLOTS:
  void apply() {
    const Vector3i dims(absOffsetX_->text().toInt(),
                        absOffsetY_->text().toInt(),
                        absOffsetZ_->text().toInt());

    vol_.Coords().SetAbsOffset(dims);

    updateLineEdits();
  }

 public:
  AbsCoordBox(OmMipVolume& vol, QWidget* parent)
      : QGroupBox(parent),
        vol_(vol),
        grid_(new QGridLayout(this)),
        absOffsetX_(new QLineEdit(this)),
        absOffsetY_(new QLineEdit(this)),
        absOffsetZ_(new QLineEdit(this)) {
    setTitle("Absolute Coordinates");

    createRows();
    updateLineEdits();
    createApplyButton();
  }

 private:
  void createApplyButton() {
    QPushButton* apply = new QPushButton("Apply");
    om::connect(apply, SIGNAL(clicked()), this, SLOT(apply()));
    grid_->addWidget(apply, 4, 1);
  }

  void createRows() {
    addRow("X offset", 0);
    grid_->addWidget(absOffsetX_, 0, 1);

    addRow("Y offset", 1);
    grid_->addWidget(absOffsetY_, 1, 1);

    addRow("Z offset", 2);
    grid_->addWidget(absOffsetZ_, 2, 1);
  }

  void addRow(const QString name, const int rowNum) {
    QLabel* label = new QLabel(this);
    label->setText(name);
    grid_->addWidget(label, rowNum, 0);
  }

  void updateLineEdits() {
    const Vector3i absOffset = vol_.Coords().GetAbsOffset();

    absOffsetX_->setText(QString::number(absOffset.x));
    absOffsetY_->setText(QString::number(absOffset.y));
    absOffsetZ_->setText(QString::number(absOffset.z));
  }
};

}  // namespace volumeInspector
}  // namespace om
