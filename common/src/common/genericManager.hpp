#pragma once

#include "common/common.h"
#include "common/string.hpp"
#include "common/exception.h"
#include "common/container.hpp"
#include "zi/mutex.hpp"

namespace YAML { class GenericManager; }

namespace om {
namespace common {

template <typename T, typename Lock = zi::spinlock>
class GenericManager {
private:
    static const uint32_t DEFAULT_MAP_SIZE = 10;

    ID nextId_;
    uint32_t size_;

    std::vector<T*> vec_;
    std::vector<T*> vecValidPtrs_;

    IDSet validSet_;  // keys in map (fast iteration)
    IDSet enabledSet_; // enabled keys in map

    Lock lock_;

public:
    GenericManager()
        : nextId_(1)
        , size_(DEFAULT_MAP_SIZE)
    {
        vec_.resize(DEFAULT_MAP_SIZE, NULL);
    }

    ~GenericManager()
    {
        for(uint32_t i = 1; i < size_; ++i){
            delete vec_[i];
        }
    }

    //managed accessors
    inline T& Add()
    {
        zi::guard g(lock_);

        const ID id = nextId_;

        T* t = new T(id);
        vec_[id] = t;
        vecValidPtrs_.push_back(t);

        findAndSetNextValidID();

        validSet_.insert(id);
        enabledSet_.insert(id);

        return *vec_[id];
    }

    inline T& Get(const ID id) const
    {
        zi::guard g(lock_);

        throwIfInvalidID(id);
        return *vec_[id];
    }

    void Remove(const ID id)
    {
        zi::guard g(lock_);

        throwIfInvalidID(id);

        validSet_.erase(id);
        enabledSet_.erase(id);

        T* t = vec_[id];

        container::eraseRemove(vecValidPtrs_, t);

        delete t;
        vec_[id] = NULL;

        findAndSetNextValidID();
    }

    //valid
    inline bool IsValid(const ID id) const
    {
        zi::guard g(lock_);
        return !isIDinvalid(id);
    }

    // TODO: Remove return of ref to ensure locking of vector is not circumvented
    inline const IDSet& GetValidIds() const
    {
        zi::guard g(lock_);
        return validSet_;
    }

    //enabled
    inline bool IsEnabled(const ID id) const
    {
        zi::guard g(lock_);
        throwIfInvalidID(id);
        return enabledSet_.count(id);
    }

    inline void SetEnabled(const ID id, const bool enable)
    {
        zi::guard g(lock_);
        throwIfInvalidID(id);

        if(enable) {
            enabledSet_.insert(id);
        } else {
            enabledSet_.erase(id);
        }
    }

    // TODO: Remove return of ref to ensure locking of vector is not circumvented
    inline const IDSet& GetEnabledIds() const
    {
        zi::guard g(lock_);
        return enabledSet_;
    }

    // This does not circumvent locking of vector; there is a possible race when deleting an object,
    //  but the deletion process must ensure all widgets, etc. are done w/ the object before
    //  proceeding
    inline const std::vector<T*> GetPtrVec() const
    {
        zi::guard g(lock_);
        return vecValidPtrs_;
    }

private:
    inline bool isIDinvalid(const ID id) const {
        return id < 1 || id >= size_ || NULL == vec_[id];
    }

    inline void throwIfInvalidID(const ID id) const
    {
        if(isIDinvalid(id)){
            assert(0 && "invalid ID");
            throw AccessException("Cannot get object with id: " + om::string::num(id));
        }
    }

    void findAndSetNextValidID()
    {
        // search to fill in holes in number map
        //  (holes could be present from object deletion...)
        for(uint32_t i = 1; i < size_; ++i )
        {
            if(NULL == vec_[i] ){
                nextId_ = i;
                return;
            }
        }

        nextId_ = size_;
        size_ *= 2;
        vec_.resize(size_, NULL);
    }

    friend class YAML::GenericManager;
};

} // namespace common
} // namespace om
