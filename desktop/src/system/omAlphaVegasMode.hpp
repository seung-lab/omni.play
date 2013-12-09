#pragma once

#include "gui/toolbars/mainToolbar/filterWidget.hpp"

#include <qthread.h>

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

      FilterWidget::Cycle();
      QThread::msleep(666);
    }
  }

 private:
  bool stop_;
};
