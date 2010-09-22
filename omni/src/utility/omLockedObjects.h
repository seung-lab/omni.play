#ifndef OM_LOCKED_OBJECTS_H
#define OM_LOCKED_OBJECTS_H

#include <zi/mutex>
#include <zi/utility>

#include <list>
#include <map>
#include <set>
#include <utility>
#include <iterator>

template <typename VAL>
class LockableList
	: public std::list<VAL>,
	  public zi::RWMutex {
};

template <typename KEY>
class LockedList{
public:
	virtual ~LockedList(){}
	bool empty(){
		zi::ReadGuard g(mutex_);
		return list_.empty();
	}
	void push_back(const KEY& key){
		zi::WriteGuard g(mutex_);
		list_.push_back(key);
	}
	void clear(){
		zi::WriteGuard g(mutex_);
		list_.clear();
	}
	void swap(std::list<KEY>& newList){
		zi::WriteGuard g(mutex_);
		list_.swap(newList);
	}
private:
	std::list<KEY> list_;
	zi::RWMutex mutex_;
};

template <typename KEY, typename VAL>
class LockedMultiMap{
public:
	typedef typename std::multimap<KEY,VAL>::iterator KViterator;
	typedef std::list<VAL> valsCont;

	virtual ~LockedMultiMap(){}
	void insert(const KEY& key, const VAL& val){
		zi::Guard g(mutex_);
		mmap_.insert(std::pair<KEY,VAL>(key,val));
	}
	void clear(){
		zi::Guard g(mutex_);
		mmap_.clear();
	}

	boost::shared_ptr<valsCont> removeKey(const KEY& key){
		zi::Guard g(mutex_);
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
	zi::Mutex mutex_;
};

#endif
