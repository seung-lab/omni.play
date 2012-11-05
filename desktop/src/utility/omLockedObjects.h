#pragma once

#include "zi/omMutex.h"
#include "zi/omUtility.h"

#include <list>
#include <map>
#include <set>
#include <utility>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/unordered_set.hpp>

template <typename VAL>
class LockedVector{
public:
    virtual ~LockedVector()
    {}

    bool empty()
    {
        zi::guard g(mutex_);
        return vector_.empty();
    }
    void clear()
    {
        zi::guard g(mutex_);
        vector_.clear();
    }
    void resize(const size_t num)
    {
        zi::guard g(mutex_);
        return vector_.resize(num);
    }
    VAL& operator[](const size_t i)
    {
        zi::guard g(mutex_);
        return vector_[i];
    }
    void swap(std::vector<VAL>& vec)
    {
        zi::guard g(mutex_);
        vector_.swap(vec);
    }
private:
    std::vector<VAL> vector_;
    zi::spinlock mutex_;
};

template <typename KEY, typename VAL>
class LockedBoostMap{
public:
    virtual ~LockedBoostMap()
    {}

    bool empty()
    {
        zi::guard g(mutex_);
        return map_.empty();
    }
    void clear()
    {
        zi::guard g(mutex_);
        map_.clear();
    }
    size_t count(const KEY& key)
    {
        zi::guard g(mutex_);
        return map_.count(key);
    }

    VAL& operator[](const KEY& key)
    {
        zi::guard g(mutex_);
        return map_[key];
    }

private:
    boost::unordered_map<KEY, VAL> map_;
    zi::spinlock mutex_;
};

template <typename KEY>
class LockedBoostSet{
public:
    virtual ~LockedBoostSet()
    {}

    bool empty()
    {
        zi::guard g(mutex_);
        return set_.empty();
    }

    void insert(const KEY& key)
    {
        zi::guard g(mutex_);
        set_.insert(key);
    }

    void erase(const KEY& key)
    {
        zi::guard g(mutex_);
        set_.erase(key);
    }

    void clear()
    {
        zi::guard g(mutex_);
        set_.clear();
    }

    bool insertSinceWasntPresent(const KEY& key)
    {
        zi::guard g(mutex_);
        if(set_.count(key)){
            return false;
        }else{
            set_.insert(key);
            return true;
        }
    }

private:
    boost::unordered_set<KEY> set_;
    zi::spinlock mutex_;
};


template <typename KEY>
class LockedSet{
public:
    virtual ~LockedSet()
    {}

    bool empty()
    {
        zi::guard g(mutex_);
        return set_.empty();
    }

    void erase(const KEY& key)
    {
        zi::guard g(mutex_);
        set_.erase(key);
    }

    void insert(const KEY& key)
    {
        zi::guard g(mutex_);
        set_.insert(key);
    }

    void clear()
    {
        zi::guard g(mutex_);
        set_.clear();
    }

    bool insertSinceWasntPresent(const KEY& key)
    {
        zi::guard g(mutex_);
        if(set_.count(key)){
            return false;
        }else{
            set_.insert(key);
            return true;
        }
    }

private:
    std::set<KEY> set_;
    zi::spinlock mutex_;
};

template <typename KEY>
class LockedList{
public:
    virtual ~LockedList(){}
    bool empty(){
        zi::guard g(mutex_);
        return list_.empty();
    }
    void push_back(const KEY& key){
        zi::guard g(mutex_);
        list_.push_back(key);
    }
    void clear(){
        zi::guard g(mutex_);
        list_.clear();
    }
    void swap(std::list<KEY>& newList){
        zi::guard g(mutex_);
        list_.swap(newList);
    }
private:
    std::list<KEY> list_;
    zi::spinlock mutex_;
};

template <typename KEY, typename VAL>
class LockedMultiMap{
public:
    typedef typename std::multimap<KEY,VAL>::iterator KViterator;
    typedef std::list<VAL> valsCont;

    virtual ~LockedMultiMap()
    {}

    void insert(const KEY& key, const VAL& val)
    {
        zi::guard g(mutex_);
        mmap_.insert(std::pair<KEY,VAL>(key,val));
    }

    void clear(){
        zi::guard g(mutex_);
        mmap_.clear();
    }

    boost::shared_ptr<valsCont> removeKey(const KEY& key)
    {
        zi::guard g(mutex_);

        boost::shared_ptr<valsCont> vals =
            om::make_shared<valsCont>();

        std::pair<KViterator, KViterator> found =
            mmap_.equal_range(key);

        KViterator it;
        for(it = found.first; it != found.second; ++it){
            vals->push_back(it->second);
        }

        mmap_.erase(key);

        return vals;
    }
private:
    std::multimap<KEY, VAL> mmap_;
    zi::spinlock mutex_;
};

