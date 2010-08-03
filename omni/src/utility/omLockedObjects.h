#ifndef OM_LOCKED_OBJECTS_H
#define OM_LOCKED_OBJECTS_H

#include <zi/mutex>
#include <zi/utility>

#include <list>

template <typename KEY, typename VAL>
class LockedCacheMap : private zi::RWMutex {
public:
	LockedCacheMap(){}
	inline bool setIfHadKey(const KEY & k, VAL & ptr){
		zi::ReadGuard g(mutex_);
		if(0 == map_.count(k)){
			return false;
		}
		ptr = map_[k];
		return true;
	}
	inline void set(const KEY k, VAL v){
		zi::WriteGuard g(mutex_);
		map_[k]=v;
	}
	inline void erase(const KEY k){
		zi::WriteGuard g(mutex_);
		map_.erase(k);
	}
	inline size_t size(){
		zi::ReadGuard g(mutex_);
		return map_.size();
	}
	inline bool empty(){
		zi::ReadGuard g(mutex_);
		return map_.empty();
	}
	inline void flush(){
		zi::WriteGuard g(mutex_);
		FOR_EACH( iter, map_ ){
			(*iter).second->Flush();
		}
	}
	inline void clear(){
		zi::WriteGuard g(mutex_);
		map_.clear();
	}
private:
	std::map<KEY, VAL> map_;
	zi::RWMutex mutex_;
};

template <typename KEY>
class LockedKeySet{
public:
	LockedKeySet(){}
	inline bool insertSinceDidNotHaveKey(const KEY k){
		zi::WriteGuard g(mutex_);
		if(set_.count(k) > 0){
			return false;
		}
		set_.insert(k);
		return true;
	}
	inline void insert(const KEY k){
		zi::WriteGuard g(mutex_);
		set_.insert(k);
	}
	inline void erase(const KEY k){
		zi::WriteGuard g(mutex_);
		set_.erase(k);
	}
private:
	std::set<KEY> set_;
	zi::RWMutex mutex_;
};

template <typename VAL>
class LockedKeyList{
public:
	LockedKeyList(){}
	inline VAL remove_back(){
		zi::WriteGuard g(mutex_);
		const VAL & ret = list_.back();
		list_.pop_back();
		return ret;
	}
	inline void touch(const VAL & val){
		zi::WriteGuard g(mutex_);
		list_.remove(val);
		list_.push_front(val);
	}
	inline bool empty(){
		zi::ReadGuard g(mutex_);
		return list_.empty();
	}
private:
	std::list<VAL> list_;
	zi::RWMutex mutex_;
};

class LockedBool{
public:
	LockedBool()
		: val_(false) {}
	inline bool get(){
		zi::ReadGuard g(mutex_);
		return val_;
	}
	inline void set(const bool b){
		zi::WriteGuard g(mutex_);
		val_ = b;
	}

private:
	bool val_;
	zi::RWMutex mutex_;
};

template <typename T>
class LockedNumber{
public:
	LockedNumber()
		: val_(0) {}
	inline T get(){
		zi::ReadGuard g(mutex_);
		return val_;
	}
	inline void set(const T val){
		zi::WriteGuard g(mutex_);
		val_ = val;
	}
	inline void add(const T val){
		zi::WriteGuard g(mutex_);
		val_ += val;
	}
	inline void sub(const T val){
		zi::WriteGuard g(mutex_);
		val_ -= val;
	}
private:
	T val_;
	zi::RWMutex mutex_;
};

typedef LockedNumber<int64_t> LockedInt64;
typedef LockedNumber<uint64_t> LockedUint64;

#endif
