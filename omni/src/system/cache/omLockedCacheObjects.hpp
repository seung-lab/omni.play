#ifndef OM_LOCKED_CACHE_OBJECTS_HPP
#define OM_LOCKED_CACHE_OBJECTS_HPP

#include "common/omCommon.h"
#include "zi/omMutex.h"
#include "zi/omUtility.h"
#include "system/cache/omCacheObjects.hpp"

template <typename KEY, typename VAL>
class LockedCacheMap {
public:
    virtual ~LockedCacheMap()
    {}
    inline bool setIfHadKey(const KEY & k, VAL & ptr)
    {
        zi::rwmutex::read_guard g(mutex_);
        if(0 == map_.count(k)){
            return false;
        }
        ptr = map_[k];
        return true;
    }
    inline void set(const KEY& k, const VAL& v)
    {
        zi::rwmutex::write_guard g(mutex_);
        map_[k] = v;
    }
    inline VAL get(const KEY& k)
    {
        zi::rwmutex::read_guard g(mutex_);
        return map_[k];
    }
    inline size_t erase(const KEY& k)
    {
        zi::rwmutex::write_guard g(mutex_);
        return map_.erase(k);
    }
    inline size_t size() const
    {
        zi::rwmutex::read_guard g(mutex_);
        return map_.size();
    }
    inline bool empty()
    {
        zi::rwmutex::read_guard g(mutex_);
        return map_.empty();
    }
    inline void clear()
    {
        zi::rwmutex::write_guard g(mutex_);
        map_.clear();
    }
    inline void swap(std::map<KEY, VAL>& newMap)
    {
        zi::rwmutex::write_guard g(mutex_);
        map_.swap(newMap);
    }
    inline bool contains(const KEY& key)
    {
        zi::rwmutex::read_guard g(mutex_);
        return map_.count(key);
    }
private:
    std::map<KEY, VAL> map_;
    zi::rwmutex mutex_;
};

template <typename KEY>
class LockedKeySet{
public:
    virtual ~LockedKeySet()
    {}
    inline bool insertSinceDidNotHaveKey(const KEY k)
    {
        zi::rwmutex::write_guard g(mutex_);
        if(set_.count(k) > 0){
            return false;
        }
        set_.insert(k);
        return true;
    }
    inline void insert(const KEY& k)
    {
        zi::rwmutex::write_guard g(mutex_);
        set_.insert(k);
    }
    inline void erase(const KEY& k)
    {
        zi::rwmutex::write_guard g(mutex_);
        set_.erase(k);
    }
    inline void clear()
    {
        zi::rwmutex::write_guard g(mutex_);
        set_.clear();
    }
private:
    std::set<KEY> set_;
    zi::rwmutex mutex_;
};

template <typename KEY>
class LockedKeyList{
public:
    virtual ~LockedKeyList()
    {}
    inline KEY remove_oldest()
    {
        zi::rwmutex::write_guard g(mutex_);
        if(list_.empty()){
            return KEY();
        }
        const KEY ret = list_.back();
        list_.pop_back();
        return ret;
    }
    inline void touch(const KEY& key)
    {
        zi::rwmutex::write_guard g(mutex_);
        list_.remove(key);
        list_.push_front(key);
    }
    inline bool empty() const
    {
        zi::rwmutex::read_guard g(mutex_);
        return list_.empty();
    }
    inline void clear()
    {
        zi::rwmutex::write_guard g(mutex_);
        list_.clear();
    }
private:
    std::list<KEY> list_;
    zi::rwmutex mutex_;
};

/**
 * based on
 *  http://www.boost.org/doc/libs/1_43_0/libs/multi_index/example/serialization.cpp
 *
 * oldest keys at front; newest at back
 *
 * pre-fectched keys will:
 *   --be added to front if key was not present in list_
 *   --not change order if key was present in list_
 **/
template <typename KEY>
class LockedKeyMultiIndex {
public:
    virtual ~LockedKeyMultiIndex()
    {}
    inline KEY remove_oldest()
    {
        zi::rwmutex::write_guard g(mutex_);
        return list_.remove_oldest();
    }
    inline void touch(const KEY& key)
    {
        zi::rwmutex::write_guard g(mutex_);
        list_.touch(key);
    }
    inline void touch(const std::list<KEY>& keys)
    {
        zi::rwmutex::write_guard g(mutex_);
        list_.touch(keys);
    }
    inline void touchPrefetch(const KEY& key)
    {
        zi::rwmutex::write_guard g(mutex_);
        list_.touchPrefetch(key);
    }
    inline bool empty() const
    {
        zi::rwmutex::read_guard g(mutex_);
        return list_.empty();
    }
    inline void clear()
    {
        zi::rwmutex::write_guard g(mutex_);
        list_.clear();
    }

private:
    zi::rwmutex mutex_;

    KeyMultiIndex<KEY> list_;
};

#endif
