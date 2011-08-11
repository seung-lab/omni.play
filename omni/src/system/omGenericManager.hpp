#pragma once

/*
 * Templated generic manager for a objects that have an
 *   (OmID id) constructor.
 *
 *  NOT thread-safe
 *
 * Brett Warne - bwarne@mit.edu - 2/20/09
 */

#include "common/omCommon.h"
#include "common/omException.h"
#include "common/omContainer.hpp"

class OmChannel;
class OmSegmentation;
class OmFilter2d;
class OmGroup;

namespace om { 
namespace data { 
namespace archive {
class genericManager;
};};};

//TODO: lock correctly

template <typename T>
class OmGenericManager {
private:
    static const uint32_t DEFAULT_MAP_SIZE = 10;

public:
    OmGenericManager()
        : nextId_(1)
        , size_(DEFAULT_MAP_SIZE)
    {
        vec_.resize(DEFAULT_MAP_SIZE, NULL);
    }

    ~OmGenericManager()
    {
        for(uint32_t i = 1; i < size_; ++i){
            delete vec_[i];
        }
    }

    //managed accessors
    inline T& Add()
    {
        const OmID id = nextId_;

        T* t = new T(id);
        vec_[id] = t;
        vecValidPtrs_.push_back(t);

        findAndSetNextValidID();

        validSet_.insert(id);
        enabledSet_.insert(id);

        return *vec_[id];
    }

    inline T& Get(const OmID id) const
    {
        throwIfInvalidID(id);
        return *vec_[id];
    }

    void Remove(const OmID id)
    {
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
    inline bool IsValid(const OmID id) const{
        return !isIDinvalid(id);
    }

    inline const OmIDsSet& GetValidIds() const{
        return validSet_;
    }

    //enabled
    inline bool IsEnabled(const OmID id) const
    {
        throwIfInvalidID(id);
        return enabledSet_.count(id);
    }

    inline void SetEnabled(const OmID id, const bool enable)
    {
        throwIfInvalidID(id);

        if(enable) {
            enabledSet_.insert(id);
        } else {
            enabledSet_.erase(id);
        }
    }

    inline const OmIDsSet& GetEnabledIds() const{
        return enabledSet_;
    }

    inline const std::vector<T*> GetPtrVec() const {
        return vecValidPtrs_;
    }

private:
    OmID nextId_;
    uint32_t size_;

    std::vector<T*> vec_;
    std::vector<T*> vecValidPtrs_;

    OmIDsSet validSet_;  // keys in map (fast iteration)
    OmIDsSet enabledSet_; // enabled keys in map

    inline bool isIDinvalid(const OmID id) const {
        return id < 1 || id >= size_ || NULL == vec_[id];
    }

    inline void throwIfInvalidID(const OmID id) const
    {
        if(isIDinvalid(id)){
            assert(0 && "invalid ID");
            throw OmAccessException("Cannot get object with id: " + id);
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

    friend class OmGenericManagerArchive;
    friend class om::data::archive::genericManager;
};

