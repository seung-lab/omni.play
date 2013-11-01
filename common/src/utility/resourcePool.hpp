#pragma once

#include <thread>
#include <chrono>
#include <mutex>
#include <deque>
#include <functional>
#include <atomic>
#include <memory>

namespace om {
namespace utility {

template <typename T> class ResourcePool {
 public:
  ResourcePool(int max,
               std::function<std::shared_ptr<T>()> generator = &default_create)
      : num_(0), max_(max), generator_(generator), closing_(false) {}
  ~ResourcePool() {
    // Will this have problems with ordering of destructors?
    closing_.store(true);
    int released = 0;
    int tries = 0;
    const int maxTries = 10;
    while (released < num_ && ++tries < maxTries) {
      {
        std::lock_guard<std::mutex> g(m_);
        released += resources_.size();
        resources_.clear();
        if (released >= num_) {
          break;
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  struct Lease {
   public:
    Lease(ResourcePool& pool, bool spin = false)
        : pool_(pool), ptr_(pool.get()) {
      while (spin && !ptr_ && !pool_.closing_) {
        ptr_ = pool_.get();
      }
    }
    ~Lease() { pool_.release(ptr_); }
    Lease(const Lease&) = delete;
    Lease& operator=(const Lease&) = delete;
    Lease(Lease&& other) {
      swap(other);
    }  // Does allowing moving cause problems?
    void swap(Lease&& other) { std::swap(ptr_, other.ptr_); }

    explicit operator T*() { return ptr_.get(); }
    explicit operator bool() { return (bool) ptr_; }
    T* get() { return ptr_.get(); }
    T* operator->() { return ptr_.get(); }

   private:
    ResourcePool& pool_;
    std::shared_ptr<T> ptr_;
  };

 private:
  static std::shared_ptr<T> default_create() { return std::make_shared<T>(); }

  std::shared_ptr<T> get() {
    if (closing_) {
      return std::shared_ptr<T>();
    }
    std::lock_guard<std::mutex> g(m_);
    if (resources_.size() == 0) {
      if (num_ >= max_) {
        return std::shared_ptr<T>();
      }
      auto ret = generator_();
      if ((bool) ret) {
        num_++;
      }
      return ret;
    }
    auto ret = resources_.front();
    resources_.pop_front();
    return ret;
  }

  void release(std::shared_ptr<T> ptr) {
    if ((bool) ptr) {
      std::lock_guard<std::mutex> g(m_);
      resources_.push_back(ptr);
    }
  }

  std::mutex m_;
  std::deque<std::shared_ptr<T>> resources_;
  int num_;
  const int max_;
  std::function<std::shared_ptr<T>()> generator_;
  std::atomic<bool> closing_;
};
}
}  // namespace om::utility::