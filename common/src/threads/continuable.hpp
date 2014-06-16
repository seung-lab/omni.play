#pragma once

#include "precomp.h"
#include "common/logging.h"

namespace om {
namespace thread {

// TODO: locking.
template <typename T>
class Continuable {
 public:
  typedef std::function<void(T)> func_t;

  virtual void AddContinuation(func_t continuation) {
    if (value_) {
      continuation(value_.get());
    }

    if (!continuation_) {
      continuation_ = continuation;
    } else {
      continuation_ =
          std::bind(both, continuation_, continuation, std::placeholders::_1);
    }
  }

 protected:
  void do_continuation(T val) {
    value_ = val;
    if (continuation_) {
      continuation_(val);
    } else {
      log_debugs << "No Continuation.";
    }
  }

  static void both(func_t a, func_t b, T val) {
    a(val);
    b(val);
  }
  func_t continuation_;
  boost::optional<T> value_;
};

template <>
class Continuable<void> {
 public:
  typedef std::function<void()> func_t;

  virtual void AddContinuation(func_t continuation) {
    if (tripped_) {
      continuation();
    }
    if (!continuation_) {
      continuation_ = continuation;
    } else {
      continuation_ = std::bind(both, continuation, continuation_);
    }
  }

 protected:
  void do_continuation() {
    if (continuation_) {
      continuation_();
    }
  }

  static void both(func_t a, func_t b) {
    a();
    b();
  }
  func_t continuation_;
  bool tripped_;
};

template <typename T>
Continuable<T>& operator>>(Continuable<T>& c,
                           typename Continuable<T>::func_t f) {
  c.AddContinuation(f);
  return c;
}
}
}  // namespace om::thread::