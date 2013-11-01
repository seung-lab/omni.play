#pragma once

#include <typeinfo>

#include <zi/mutex.hpp>
#include "utility/malloc.hpp"

namespace om {
namespace tile {

template <class T, uint32_t NumEle, uint32_t NumTiles> class Cache {
  std::array<std::array<T, NumEle>, NumTiles> cache_;

  zi::spinlock lock_;
  std::vector<uint32_t> avail_;

  std::pair<bool, uint32_t> getNextAvailIdx() {
    zi::guard g(lock_);
    if (avail_.empty()) {
      return { false, 0 };
    }
    auto idx = avail_.back();
    avail_.pop_back();
    return { true, idx };
  }

 public:
  Cache() {
    //std::cout << "started cache: " << typeid(T).name() << std::endl;

    zi::guard g(lock_);
    avail_.reserve(NumTiles);
    for (uint32_t i = 0; i < NumTiles; ++i) {
      avail_.push_back(i);
    }
  }

  std::shared_ptr<T> get() {
    auto ifidx = getNextAvailIdx();
    if (!ifidx.first) {
      return om::mem::Malloc<T>::NumElementsDontZero(NumEle);
    }
    auto idx = ifidx.second;
    auto& arr = cache_[idx];
    auto* ptr = &arr[0];

      //std::cout << "allocating idx: " << idx << std::endl;

    return std::shared_ptr<T>(ptr, [this, idx](T*) {
      this->done(idx);
    });
  }

  void done(uint32_t idx) {
    zi::guard g(lock_);
    avail_.push_back(idx);
    //std::cout << "released idx: " << idx << std::endl;
  }
};

template <class T> std::shared_ptr<T> Make() {
  static Cache<T, 128 * 128, 1000> cache;
  return cache.get();
}

}
}  // namespace
