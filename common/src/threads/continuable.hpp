#pragma once

#include "precomp.h"

namespace om {
namespace thread {

// TODO: locking.
template <typename T>
class Continuable {
  typedef std::function<void(T)> func_t;

 public:
  virtual void AddContinuation(func_t continuation) {
    if (!continuation_) {
      continuation_ = continuation;
    } else {
      continuation_ = std::bind(both, continuation, continuation_, _1);
    }
  }

 protected:
  void do_continuation(T val) { continuation_(val); }

  static void both(func_t a, func_t b, T val) {
    a(val);
    b(val);
  }
  func_t continuation_;
};
}
}  // namespace om::thread::