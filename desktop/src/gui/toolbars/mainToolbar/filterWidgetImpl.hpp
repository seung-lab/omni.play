#pragma once
#include "precomp.h"

#include "common/common.h"
#include "events/events.h"
#include "project/omProject.h"
#include "system/omConnect.hpp"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

class FilterWidgetImpl : public QSlider {
  Q_OBJECT;

 private:
  constexpr static const double delta_ = 0.5;

  double previousAlpha_;

 public:
  FilterWidgetImpl() : QSlider(Qt::Horizontal), previousAlpha_(0.0) {
    QSize size = sizeHint();
    size.setWidth(150);
    setMaximumSize(size);

    initSilderTab();

    om::connect(this, SIGNAL(valueChanged(int)), this, SLOT(setFilAlpha(int)));

    om::connect(this, SIGNAL(signalIncreaseAlpha()), this,
                SLOT(increaseAlphaSlot()));

    om::connect(this, SIGNAL(signalDecreaseAlpha()), this,
                SLOT(decreaseAlphaSlot()));

    om::connect(this, SIGNAL(signalCycleAlpha()), this, SLOT(cycleAlphaSlot()));
  }

  void IncreaseAlpha() { signalIncreaseAlpha(); }

  void DecreaseAlpha() { signalDecreaseAlpha(); }

  void Cycle() { signalCycleAlpha(); }

Q_SIGNALS:
  void signalIncreaseAlpha();
  void signalDecreaseAlpha();
  void signalCycleAlpha();

 private
Q_SLOTS:
  void increaseAlphaSlot() { increaseAlpha(); }

  void decreaseAlphaSlot() { decreaseAlpha(); }

  void cycleAlphaSlot() { cycle(); }

 private:
  void increaseAlpha() {
    const boost::optional<double> alpha = doGetFilterAlpha();
    if (!alpha) {
      return;
    }

    double newAlpha = *alpha + delta_;

    if (newAlpha > 1) {
      newAlpha = 1;
    }

    doSetFilterAlpha(newAlpha);
  }

  void decreaseAlpha() {
    const boost::optional<double> alpha = doGetFilterAlpha();
    if (!alpha) {
      return;
    }

    double newAlpha = *alpha - delta_;

    if (newAlpha < 0) {
      newAlpha = 0;
    }

    doSetFilterAlpha(newAlpha);
  }

  void cycle() {
    const boost::optional<double> alpha = doGetFilterAlpha();
    if (!alpha) {
      return;
    }

    double newAlpha;
    if (previousAlpha_) {
      newAlpha = previousAlpha_;
    } else {
      previousAlpha_= filter->GetAlpha();
      newAlpha = 0;
    }

    doSetFilterAlpha(newAlpha);
  }

 private
Q_SLOTS:
  // sliderVal is from 0 to 100
  void setFilAlpha(const int sliderVal) {
    const double alpha = sliderVal / 100.0;
    doSetFilterAlpha(alpha);
  }

 private:
  om::common::ID getChannelID() { return 1; }

  om::common::ID getFilterID() { return 1; }

  void initSilderTab() {
    const boost::optional<double> alpha = doGetFilterAlpha();

    if (alpha) {
      moveSliderTab(*alpha);
    }
  }

  void moveSliderTab(const double alpha) {
    this->setValue(alpha * 100);
    om::event::Redraw2d();
  }

  boost::optional<double> doGetFilterAlpha() {
    FilterDataWrapper fdw(getChannelID(), getFilterID());

    if (fdw.isValid()) {
      OmFilter2d* filter = fdw.getFilter();
      return boost::optional<double>(filter->GetAlpha());
    }

    return boost::optional<double>();
  }

  void doSetFilterAlpha(const double alpha) {
    FilterDataWrapper fdw(getChannelID(), getFilterID());

    if (fdw.isValid()) {
      OmFilter2d* filter = fdw.getFilter();
      filter->SetAlpha(alpha);
      moveSliderTab(alpha);
    }
  }
};
