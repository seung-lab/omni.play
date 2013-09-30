#pragma once

#include "common/common.h"
#include "common/exception.h"
#include "common/container.hpp"
#include "zi/mutex.hpp"

class OmChannel;
class OmSegmentation;
class OmFilter2d;
class OmGroup;

namespace YAML {
class genericManager;
}

template <typename T, typename Lock = zi::spinlock> class OmGenericManager {
 protected:
  static const uint32_t DEFAULT_MAP_SIZE = 10;

  om::common::ID nextId_;
  uint32_t size_;

  std::vector<T*> vec_;
  std::vector<T*> vecValidPtrs_;

  om::common::IDSet validSet_;    // keys in map (fast iteration)
  om::common::IDSet enabledSet_;  // enabled keys in map

  Lock lock_;

 public:
  OmGenericManager() : nextId_(1), size_(DEFAULT_MAP_SIZE) {
    vec_.resize(DEFAULT_MAP_SIZE, NULL);
  }

  ~OmGenericManager() {
    for (uint32_t i = 1; i < size_; ++i) {
      delete vec_[i];
    }
  }

  //managed accessors
  inline T& Add() {
    zi::guard g(lock_);

    const om::common::ID id = nextId_;

    T* t = new T(id);
    vec_[id] = t;
    vecValidPtrs_.push_back(t);

    findAndSetNextValidID();

    validSet_.insert(id);
    enabledSet_.insert(id);

    return *vec_[id];
  }

  inline T& Get(const om::common::ID id) const {
    zi::guard g(lock_);

    throwIfInvalidID(id);
    return *vec_[id];
  }

  void Remove(const om::common::ID id) {
    zi::guard g(lock_);

    throwIfInvalidID(id);

    validSet_.erase(id);
    enabledSet_.erase(id);

    T* t = vec_[id];

    om::container::eraseRemove(vecValidPtrs_, t);

    delete t;
    vec_[id] = NULL;

    findAndSetNextValidID();
  }

  //valid
  inline bool IsValid(const om::common::ID id) const {
    zi::guard g(lock_);
    return !isIDinvalid(id);
  }

  // TODO: Remove return of ref to ensure locking of vector is not circumvented
  inline const om::common::IDSet& GetValidIds() const {
    zi::guard g(lock_);
    return validSet_;
  }

  //enabled
  inline bool IsEnabled(const om::common::ID id) const {
    zi::guard g(lock_);
    throwIfInvalidID(id);
    return enabledSet_.count(id);
  }

  inline void SetEnabled(const om::common::ID id, const bool enable) {
    zi::guard g(lock_);
    throwIfInvalidID(id);

    if (enable) {
      enabledSet_.insert(id);
    } else {
      enabledSet_.erase(id);
    }
  }

  // TODO: Remove return of ref to ensure locking of vector is not circumvented
  inline const om::common::IDSet& GetEnabledIds() const {
    zi::guard g(lock_);
    return enabledSet_;
  }

  // This does not circumvent locking of vector; there is a possible race when
  // deleting an object,
  //  but the deletion process must ensure all widgets, etc. are done w/ the
  // object before
  //  proceeding
  inline const std::vector<T*> GetPtrVec() const {
    zi::guard g(lock_);
    return vecValidPtrs_;
  }

 private:
  inline bool isIDinvalid(const om::common::ID id) const {
    return id < 1 || id >= size_ || NULL == vec_[id];
  }

  inline void throwIfInvalidID(const om::common::ID id) const {
    if (isIDinvalid(id)) {
      assert(0 && "invalid ID");
      throw om::AccessException("Cannot get object with id: " + id);
    }
  }

  void findAndSetNextValidID() {
    // search to fill in holes in number map
    //  (holes could be present from object deletion...)
    for (uint32_t i = 1; i < size_; ++i) {
      if (NULL == vec_[i]) {
        nextId_ = i;
        return;
      }
    }

    nextId_ = size_;
    size_ *= 2;
    vec_.resize(size_, NULL);
  }

  friend class OmGenericManagerArchive;
  friend class YAML::genericManager;
};
