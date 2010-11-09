#ifndef OM_LOCKED_OBJECTS_H
#define OM_LOCKED_OBJECTS_H

#include "zi/omMutex.h"
#include "zi/omUtility.h"

#include <list>
#include <map>
#include <set>
#include <utility>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

template <typename VAL>
class LockableList
	: public std::list<VAL>,
	  public zi::rwmutex {
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
	zi::mutex mutex_;
};

template <typename KEY, typename VAL>
class LockedMultiMap{
public:
	typedef typename std::multimap<KEY,VAL>::iterator KViterator;
	typedef std::list<VAL> valsCont;

	virtual ~LockedMultiMap(){}
	void insert(const KEY& key, const VAL& val){
		zi::guard g(mutex_);
		mmap_.insert(std::pair<KEY,VAL>(key,val));
	}
	void clear(){
		zi::guard g(mutex_);
		mmap_.clear();
	}

	boost::shared_ptr<valsCont> removeKey(const KEY& key){
		zi::guard g(mutex_);
		boost::shared_ptr<valsCont> vals =
			boost::make_shared<valsCont>();

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
	zi::mutex mutex_;
};

#endif
