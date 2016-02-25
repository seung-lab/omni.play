#pragma once
#include "precomp.h"

#include "gui/toolbars/mainToolbar/filterWidget.hpp"

class OmAlphaVegasMode : public QThread {
 public:
  OmAlphaVegasMode() : stop_(false) { start(); }

  ~OmAlphaVegasMode() {
    stop_ = true;
    wait();
  }

  void run() {
    Q_FOREVER {
      if (stop_) {
        return;
      }

      FilterWidget::Toggle();
      QThread::msleep(666);
    }
  }

 private:
  bool stop_;
};
