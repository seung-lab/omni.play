#pragma once
#include "precomp.h"

#include "utility/segmentationDataWrapper.hpp"
#include "gui/inspectors/inspectorProperties.h"
#include "gui/inspectors/segmentation/pageSelector.hpp"
#include "gui/inspectors/segmentation/pagesWidget.h"

// based off
// http://doc.qt.nokia.com/latest/dialogs-configdialog-configdialog-cpp.html

class SegmentationInspector : public QDialog {
  Q_OBJECT;

 private:
  InspectorProperties* const parentDialog_;
  const SegmentationDataWrapper sdw_;

  om::segmentationInspector::PageSelector* pageSelector_;
  om::segmentationInspector::PagesWidget* pagesWidget_;

 public:
  SegmentationInspector(QWidget* parent, InspectorProperties* parentDialog,
                        const SegmentationDataWrapper& sdw)
      : QDialog(parent), parentDialog_(parentDialog), sdw_(sdw) {
    pageSelector_ = new om::segmentationInspector::PageSelector(this);

    om::connect(pageSelector_, SIGNAL(RowChanged(int)), this,
                SLOT(changePage(int)));

    pagesWidget_ = new om::segmentationInspector::PagesWidget(this, sdw_);

    setupLayouts();
  }

  QString Title() {
    return QString("Segmentation %1 Inspector").arg(sdw_.GetID());
  }

 private
Q_SLOTS:
  void changePage(const int row) { pagesWidget_->setCurrentIndex(row); }

  void closeDialog() { parentDialog_->CloseDialog(); }

 private:
  void setupLayouts() {
    QPushButton* closeButton = new QPushButton("Close");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));

    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(pageSelector_);
    horizontalLayout->addWidget(pagesWidget_, 1);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
  }
};
