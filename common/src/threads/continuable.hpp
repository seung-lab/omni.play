#pragma once

#include "precomp.h"

namespace om {
namespace thread {

// TODO: locking.
template <typename T>
class Continuable {
 public:
  typedef std::function<void(T)> func_t;

  virtual void AddContinuation(func_t continuation) {
    if (!continuation_) {
      continuation_ = continuation;
    } else {
      continuation_ =
          std::bind(both, continuation_, continuation, std::placeholders::_1);
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

template <>
class Continuable<void> {
 public:
  typedef std::function<void()> func_t;

  virtual void AddContinuation(func_t continuation) {
    if (!continuation_) {
      continuation_ = continuation;
    } else {
      continuation_ = std::bind(both, continuation, continuation_);
    }
  }

 protected:
  void do_continuation() { continuation_(); }

  static void both(func_t a, func_t b) {
    a();
    b();
  }
  func_t continuation_;
};

template <typename T>
Continuable<T>& operator>>(Continuable<T>& c,
                           typename Continuable<T>::func_t f) {
  c.AddContinuation(f);
  return c;
}
}
}  // namespace om::thread::