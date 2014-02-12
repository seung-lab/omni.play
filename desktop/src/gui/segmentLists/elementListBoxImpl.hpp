#pragma once

#include "actions/omSelectSegmentParams.hpp"
#include "common/logging.h"
#include "events/details/segmentEvent.h"
#include "events/listeners.h"
#include "gui/segmentLists/details/segmentListRecent.h"
#include "gui/segmentLists/details/segmentListUncertain.h"
#include "gui/segmentLists/details/segmentListValid.h"
#include "gui/segmentLists/details/segmentListWorking.h"
#include "gui/segmentLists/omSegmentListWidget.h"
#include "gui/segmentLists/segmentListKeyPressEventListener.h"
#include "gui/widgets/progressBar.hpp"
#include "segment/omSegmentCenter.hpp"
#include "segment/omSegmentUtils.hpp"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

#include <QtGui>
#include <QWidget>

class OmViewGroupState;

class ElementListBoxImpl : public QGroupBox,
                           public om::event::SegmentEventListener,
                           public SegmentListKeyPressEventListener {
  Q_OBJECT;

 protected:
  void keyPressEvent(QKeyEvent* event) {
    SegmentListKeyPressEventListener::keyPressEvent(event);
  }

 private:
  OmViewGroupState& vgs_;

  QTabWidget* mDataElementsTabs;
  QVBoxLayout* mOverallContainer;
  om::gui::progressBar<uint64_t>* percentValidated_;

  int mCurrentlyActiveTab;

  SegmentListRecent* recentList_;
  SegmentListUncertain* uncertainList_;
  SegmentListValid* validList_;
  SegmentListWorking* workingList_;

  SegmentationDataWrapper sdw_;
  bool haveValidSDW_;

  virtual void SegmentGUIlistEvent(om::event::SegmentEvent* event) {
    const SegmentationDataWrapper& sdw = event->GUIparams().sdw;

    if (event->GUIparams().stayOnPage) {
      RefreshPage(sdw);

    } else {
      makeSegmentationActive(sdw);
    }
  }

  bool shouldScroll(const OmSelectSegmentsParams& params) {
    if (params.comment == OmSegmentListWidget::eventSenderName()) {
      // if we sent event, don't scroll
      return false;
    }

    return params.shouldScroll;
  }

  virtual void SegmentModificationEvent(om::event::SegmentEvent* event) {
    if (!event->Params().shouldScroll) {
      return;
    }

    reset();
    setTitle("");

    const OmSelectSegmentsParams& params = event->Params();

    const bool doScroll = shouldScroll(params);

    const SegmentDataWrapper& sdw = params.sdw;

    if (sdw.IsSegmentationValid()) {
      sdw_ = sdw.MakeSegmentationDataWrapper();
      haveValidSDW_ = true;

      workingList_->makeSegmentationActive(sdw, doScroll);
      validList_->makeSegmentationActive(sdw, doScroll);
      uncertainList_->makeSegmentationActive(sdw, doScroll);
      recentList_->makeSegmentationActive(sdw, doScroll);

    } else {
      if (haveValidSDW_) {
        workingList_->populate();
        validList_->populate();
        uncertainList_->populate();
        recentList_->populate();
      }
    }

    if (sdw_.IsSegmentationValid()) {
      setTitle(GetSegmentationGroupBoxTitle(sdw_));
      updateValidBar(sdw_);

      if (params.autoCenter) {
        OmSegmentCenter::CenterSegment(vgs_);
      }
    }
  }

  void SegmentSelectedEvent(om::event::SegmentEvent*) {}

  void updateValidBar(const SegmentationDataWrapper& sdw) {
    const uint64_t valid =
        sdw.SegmentLists()->NumVoxels(om::common::SegListType::VALID);

    percentValidated_->SetMaximum(sdw.SegmentLists()->TotalNumVoxels());
    percentValidated_->SetValue(valid);
  }

  QString GetSegmentationGroupBoxTitle(SegmentationDataWrapper sdw) {
    return QString("Segmentation %1: Segments").arg(sdw.id());
  }

 public:
  ElementListBoxImpl(OmViewGroupState& vgs)
      : QGroupBox(""),
        vgs_(vgs),
        mCurrentlyActiveTab(-1),
        haveValidSDW_(false) {
    workingList_ = new SegmentListWorking(this, vgs);
    validList_ = new SegmentListValid(this, vgs);
    recentList_ = new SegmentListRecent(this, vgs);
    uncertainList_ = new SegmentListUncertain(this, vgs);

    mDataElementsTabs = new QTabWidget(this);
    mOverallContainer = new QVBoxLayout(this);
    mOverallContainer->addWidget(mDataElementsTabs);

    percentValidated_ = new om::gui::progressBar<uint64_t>(this);
    mOverallContainer->addWidget(percentValidated_);
  }

  void reset() {
    mDataElementsTabs->clear();
    setTitle("");
  }

  void setActiveTab(QWidget* tab) {
    mCurrentlyActiveTab = mDataElementsTabs->indexOf(tab);
  }

  void addTab(const int preferredIndex, QWidget* tab, const QString& tabTitle) {
    if (-1 != mDataElementsTabs->indexOf(tab)) {
      return;  // tab was already added, don't add again
    }

    const int insertedIndex =
        mDataElementsTabs->insertTab(preferredIndex, tab, tabTitle);

    if (-1 == mCurrentlyActiveTab) {  // first time here
      mCurrentlyActiveTab = insertedIndex;
    }

    // keep the tab widget fixed on the user-specified tab
    mDataElementsTabs->setCurrentIndex(mCurrentlyActiveTab);
  }

  void PopulateLists() {
    workingList_->populate();
    validList_->populate();
    recentList_->populate();
    uncertainList_->populate();
  }

  void RebuildLists(const SegmentDataWrapper& sdw) {
    workingList_->rebuildSegmentList(sdw);
    validList_->rebuildSegmentList(sdw);
    recentList_->rebuildSegmentList(sdw);
    uncertainList_->rebuildSegmentList(sdw);

    updateValidBar(sdw.MakeSegmentationDataWrapper());
  }

  void UpdateSegmentListBox(const SegmentationDataWrapper& sdw) {
    reset();
    setTitle(GetSegmentationGroupBoxTitle(sdw));
    makeSegmentationActive(sdw);

    updateValidBar(sdw);
  }

  void RefreshPage(const SegmentationDataWrapper& sdw) {
    workingList_->RefreshPage(sdw);
    validList_->RefreshPage(sdw);
    recentList_->RefreshPage(sdw);
    uncertainList_->RefreshPage(sdw);

    updateValidBar(sdw);
  }

 private:
  void makeSegmentationActive(const SegmentationDataWrapper& sdw) {
    workingList_->MakeSegmentationActive(sdw);
    validList_->MakeSegmentationActive(sdw);
    recentList_->MakeSegmentationActive(sdw);
    uncertainList_->MakeSegmentationActive(sdw);

    updateValidBar(sdw);
  }
};
