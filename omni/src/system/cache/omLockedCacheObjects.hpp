#ifndef OM_LOCKED_CACHE_OBJECTS_HPP
#define OM_LOCKED_CACHE_OBJECTS_HPP

#include <zi/mutex>
#include <zi/utility>

#include <list>
#include <map>
#include <set>
#include <utility>
#include <iterator>
template <typename KEY, typename VAL>
class LockedCacheMap {
public:
	virtual ~LockedCacheMap(){}
	bool setIfHadKey(const KEY & k, VAL & ptr){
		zi::ReadGuard g(mutex_);
		if(0 == map_.count(k)){
			return false;
		}
		ptr = map_[k];
		return true;
	}
	void set(const KEY& k, const VAL& v){
		zi::WriteGuard g(mutex_);
		map_[k]=v;
	}
	void erase(const KEY& k){
		zi::WriteGuard g(mutex_);
		map_.erase(k);
	}
	size_t size(){
		zi::ReadGuard g(mutex_);
		return map_.size();
	}
	bool empty(){
		zi::ReadGuard g(mutex_);
		return map_.empty();
	}
	void flush(){
		zi::WriteGuard g(mutex_);
		FOR_EACH( iter, map_ ){
			(*iter).second->Flush();
		}
	}
	void clear(){
		zi::WriteGuard g(mutex_);
		map_.clear();
	}
	void swap(std::map<KEY, VAL>& newMap){
		zi::WriteGuard g(mutex_);
		map_.swap(newMap);
	}
private:
	std::map<KEY, VAL> map_;
	zi::RWMutex mutex_;
};

template <typename KEY>
class LockedKeySet{
public:
	virtual ~LockedKeySet(){}
	bool insertSinceDidNotHaveKey(const KEY k){
		zi::Guard g(mutex_);
		if(set_.count(k) > 0){
			return false;
		}
		set_.insert(k);
		return true;
	}
	void insert(const KEY& k){
		zi::Guard g(mutex_);
		set_.insert(k);
	}
	void erase(const KEY& k){
		zi::Guard g(mutex_);
		set_.erase(k);
	}
	void clear(){
		zi::Guard g(mutex_);
		set_.clear();
	}
private:
	std::set<KEY> set_;
	zi::Mutex mutex_;
};

template <typename VAL>
class LockedKeyList{
public:
	virtual ~LockedKeyList(){}
	VAL remove_back(){
		zi::WriteGuard g(mutex_);
		const VAL & ret = list_.back();
		list_.pop_back();
		return ret;
	}
	void touch(const VAL& val){
		zi::WriteGuard g(mutex_);
		list_.remove(val);
		list_.push_front(val);
	}
	bool empty(){
		zi::ReadGuard g(mutex_);
		return list_.empty();
	}
	void clear(){
		zi::WriteGuard g(mutex_);
		list_.clear();
	}
private:
	std::list<VAL> list_;
	zi::RWMutex mutex_;
};
#endif
