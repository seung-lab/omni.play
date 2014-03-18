#pragma once
#include "precomp.h"

#include "utility/channelDataWrapper.hpp"
#include "gui/inspectors/inspectorProperties.h"
#include "gui/inspectors/channel/pageSelectorChannel.hpp"
#include "gui/inspectors/channel/pagesWidgetChannel.h"

// based off
// http://doc.qt.nokia.com/latest/dialogs-configdialog-configdialog-cpp.html

class ChannelInspector : public QDialog {
  Q_OBJECT;

 private:
  InspectorProperties* const parentDialog_;
  const ChannelDataWrapper cdw_;

  om::channelInspector::PageSelector* pageSelector_;
  om::channelInspector::PagesWidget* pagesWidget_;

 public:
  ChannelInspector(QWidget* parent, InspectorProperties* parentDialog,
                   const ChannelDataWrapper& cdw)
      : QDialog(parent), parentDialog_(parentDialog), cdw_(cdw) {
    pageSelector_ = new om::channelInspector::PageSelector(this);

    om::connect(pageSelector_, SIGNAL(RowChanged(int)), this,
                SLOT(changePage(int)));

    pagesWidget_ = new om::channelInspector::PagesWidget(this, cdw_);

    setupLayouts();
  }

  QString Title() { return QString("Channel %1 Inspector").arg(cdw_.GetID()); }

  ChannelDataWrapper Channel() const { return cdw_; }

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
