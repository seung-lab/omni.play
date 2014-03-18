#pragma once
#include "precomp.h"

#include "segment/omSegmentSelected.hpp"

class SegmentListKeyPressEventListener {
 public:
  SegmentListKeyPressEventListener() {}

  void keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
      case Qt::Key_Down:
        log_infos << "segment list key down";
        break;
      case Qt::Key_Up:
        log_infos << "segment list key up";
        break;
    }
  }
};
