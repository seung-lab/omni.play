#pragma once

#include "precomp.h"

namespace om {
namespace thread {

class Waitable {
 public:
  Waitable(bool ready = false) ready_(ready) {}

  bool is_ready() { return ready_.load(); }
  void wait() {
    std::unique_lock<std::mutex> lk(m_);
    cv_.wait(lk, [&]() { return ready_.load(); });
  }

  template <typename Rep, typename Period>
  void wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
    std::unique_lock<std::mutex> lk(m_);
    cv_.wait_for(lk, rel_time, [&]() { return ready_.load(); });
  }

  template <typename Clock, typename Duration>
  void wait_until(
      const std::chrono::time_point<Clock, Duration>& timeout_time) {
    std::unique_lock<std::mutex> lk(m_);
    cv_.wait_until(lk, timeout_time, [&]() { return ready_.load(); });
  }

 protected:
  void set_ready() {
    ready_.store(true);
    cv_.notify_all();
  }

  std::mutex m_;
  std::condition_variable cv_;
  std::atomic<bool> ready_;
};
}
}  // namespace om::thread::