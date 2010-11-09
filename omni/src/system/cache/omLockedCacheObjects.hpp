#ifndef OM_LOCKED_CACHE_OBJECTS_HPP
#define OM_LOCKED_CACHE_OBJECTS_HPP

#include "common/omCommon.h"
#include "zi/omMutex.h"
#include "zi/omUtility.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>

template <typename KEY, typename VAL>
class LockedCacheMap {
public:
	virtual ~LockedCacheMap(){}
	bool setIfHadKey(const KEY & k, VAL & ptr){
		zi::rwmutex::read_guard g(mutex_);
		if(0 == map_.count(k)){
			return false;
		}
		ptr = map_[k];
		return true;
	}
	void set(const KEY& k, const VAL& v){
		zi::rwmutex::write_guard g(mutex_);
		map_[k]=v;
	}
	VAL get(const KEY& k){
		zi::rwmutex::read_guard g(mutex_);
		return map_[k];
	}
	void erase(const KEY& k){
		zi::rwmutex::write_guard g(mutex_);
		map_.erase(k);
	}
	size_t size(){
		zi::rwmutex::read_guard g(mutex_);
		return map_.size();
	}
	bool empty(){
		zi::rwmutex::read_guard g(mutex_);
		return map_.empty();
	}
	void clear(){
		zi::rwmutex::write_guard g(mutex_);
		map_.clear();
	}
	void swap(std::map<KEY, VAL>& newMap){
		zi::rwmutex::write_guard g(mutex_);
		map_.swap(newMap);
	}
	bool contains(const KEY& key){
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
	virtual ~LockedKeySet(){}
	bool insertSinceDidNotHaveKey(const KEY k){
		zi::guard g(mutex_);
		if(set_.count(k) > 0){
			return false;
		}
		set_.insert(k);
		return true;
	}
	void insert(const KEY& k){
		zi::guard g(mutex_);
		set_.insert(k);
	}
	void erase(const KEY& k){
		zi::guard g(mutex_);
		set_.erase(k);
	}
	void clear(){
		zi::guard g(mutex_);
		set_.clear();
	}
private:
	std::set<KEY> set_;
	zi::mutex mutex_;
};

template <typename KEY>
class LockedKeyList{
public:
	virtual ~LockedKeyList(){}
	KEY remove_oldest(){
		zi::rwmutex::write_guard g(mutex_);
		if(list_.empty()){
			return KEY();
		}
		const KEY ret = list_.back();
		list_.pop_back();
		return ret;
	}
	void touch(const KEY& key){
		zi::rwmutex::write_guard g(mutex_);
		list_.remove(key);
		list_.push_front(key);
	}
	bool empty() const {
		zi::rwmutex::read_guard g(mutex_);
		return list_.empty();
	}
	void clear(){
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
	KEY remove_oldest(){
		zi::guard g(mutex_);
		if(list_.empty()){
			return KEY();
		}
		const KEY ret = list_.front();
		list_.pop_front();
		return ret;
	}
	void touch(const KEY& key){
		zi::guard g(mutex_);
		std::pair<iterator, bool> p = list_.push_back(key);
		if(!p.second){ // key already in list
			list_.relocate(list_.end(), p.first);
		}
	}
	void touchPrefetch(const KEY& key){
		zi::guard g(mutex_);
		// add to front (make LRU), or don't adjust position
		list_.push_front(key);
	}
	bool empty() const {
		zi::guard g(mutex_);
		return list_.empty();
	}
	void clear(){
		zi::guard g(mutex_);
		list_.clear();
	}

private:
	zi::mutex mutex_;

	typedef boost::multi_index::multi_index_container<
		KEY,
		boost::multi_index::indexed_by<
			boost::multi_index::sequenced<>,
			boost::multi_index::ordered_unique<boost::multi_index::identity<KEY> >
			>
		> lru_list;

	typedef typename lru_list::iterator iterator;

	lru_list list_;
};

#endif
