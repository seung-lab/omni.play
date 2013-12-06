#pragma once

class OmMouseEventState {
 public:
  template <class T>
  static void Print(T* target) {
    log_info(
        "controlKey_(%d), altKey_(%d), shiftKey_(%d), leftMouseButton_(%d), "
        "rightMouseButton_(%d), ",
        target->controlKey_, target->altKey_, target->shiftKey_,
        target->leftMouseButton_, target->rightMouseButton_)
        << target->tool_ << "click point: " << target->dataClickPoint_;
  }
};
