#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <zi/for_each.hpp>

template <typename KEY> class KeyMultiIndex {
 public:
  virtual ~KeyMultiIndex() {}

  inline void swap(KeyMultiIndex<KEY>& newList) { list_.swap(newList.list_); }

  inline KEY remove_oldest() {
    if (list_.empty()) {
      return KEY();
    }
    const KEY ret = list_.front();
    list_.pop_front();
    return ret;
  }

  inline void touch(const KEY& key) {
    std::pair<iterator, bool> p = list_.push_back(key);
    if (!p.second)  // key already in list
        {
      list_.relocate(list_.end(), p.first);
    }
  }

  inline void touch(const std::list<KEY>& keys) {
    FOR_EACH(iter, keys) {
      std::pair<iterator, bool> p = list_.push_back(*iter);
      if (!p.second) {  // key already in list
        list_.relocate(list_.end(), p.first);
      }
    }
  }

  inline void touch(const std::deque<KEY>& keys) {
    FOR_EACH(iter, keys) {
      std::pair<iterator, bool> p = list_.push_back(*iter);
      if (!p.second) {  // key already in list
        list_.relocate(list_.end(), p.first);
      }
    }
  }

  inline void touchPrefetch(const KEY& key) {
    // add to front (make LRU), or don't adjust position
    list_.push_front(key);
  }

  inline bool empty() const { return list_.empty(); }

  inline void clear() { list_.clear(); }

  void Dump(std::vector<KEY>& vec) {
    vec.reserve(list_.size());

    FOR_EACH(iter, list_) { vec.push_back(*iter); }
  }

  size_t Size() { return list_.size(); }

 private:
  typedef boost::multi_index::multi_index_container<
      KEY, boost::multi_index::indexed_by<
               boost::multi_index::sequenced<>,
               boost::multi_index::ordered_unique<
                   boost::multi_index::identity<KEY> > > > lru_list;

  typedef typename lru_list::iterator iterator;

  lru_list list_;
};
