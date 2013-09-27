#pragma once

#include <zi/bits/unordered_map.hpp>
#include <zi/bits/unordered_set.hpp>
#include <zi/concurrency.hpp>
#include <zi/utility/non_copyable.hpp>
#include <zi/utility/assert.hpp>

#include <cstddef>

namespace zi {
namespace detail {

template <class T>
class rwlock_impl : zi::non_copyable {
 private:
  typedef T client_type;
  typedef zi::unordered_map<T, std::size_t> client_set;

  mutable bool writer_waiting_;
  mutable bool has_writer_;

  mutable client_set readers_;
  mutable client_type writer_;

  zi::condition_variable reader_cv_;
  zi::condition_variable writer_cv_;

 public:
  rwlock_impl()
      : writer_waiting_(false),
        has_writer_(false),
        readers_(),
        writer_(),
        reader_cv_(),
        writer_cv_() {}

  bool has_readers() const { return readers_.size() > 0; }

  bool has_writers() const { return has_writer_; }

  std::size_t num_readers() const { return readers_.size(); }

  std::size_t num_writers() const { return has_writer_ > 0 ? 1 : 0; }

  std::size_t num_clients() const { return num_readers() + num_writers(); }

  bool try_acquire_read(const T& client) const {
    if (has_writer_ || writer_waiting_) {
      return false;
    }

    ++readers_[client];
    return true;
  }

  void acquire_read(const T& client, const zi::mutex& m) const {
    while (has_writer_ || writer_waiting_) {
      reader_cv_.wait(m);
    }
    ++readers_[client];
  }

  inline bool timed_acquire_read(const T& client, const zi::mutex& m,
                                 int64_t ttl) const {
    while (has_writer_ || writer_waiting_) {
      if (!reader_cv_.timed_wait(m, ttl)) {
        return false;
      }
    }
    ++readers_[client];
    return true;
  }

  template <int64_t I>
  inline bool timed_acquire_read(
      const T& client, const zi::mutex& m,
      const zi::interval::detail::interval_tpl<I>& ttl) const {
    while (has_writer_ || writer_waiting_) {
      if (!reader_cv_.timed_wait(m, ttl)) {
        return false;
      }
    }

    ++readers_[client];
    return true;
  }

  inline void release_read(const T& client) const {
    ZI_ASSERT(readers_.count(client) > 0);

    typename client_set::iterator it = readers_.find(client);
    --it->second;

    if (it->second == 0) {
      readers_.erase(it);
    }

    if (!readers_.size()) {
      writer_waiting_ = false;
      writer_cv_.notify_one();
      reader_cv_.notify_all();
    }
  }

  inline bool try_acquire_write(const T& client) const {
    if (readers_.size() || has_writer_) {
      return false;
    }

    has_writer_ = true;
    writer_ = client;
    return true;
  }

  inline void acquire_write(const T& client, const zi::mutex& m) const {
    while (readers_.size() || has_writer_) {
      writer_waiting_ = true;
      writer_cv_.wait(m);
    }

    writer_ = client;
    has_writer_ = true;
  }

  inline void release_write(const T& client) const {
    ZI_VERIFY(client == writer_);

    has_writer_ = writer_waiting_ = false;
    writer_cv_.notify_one();
    reader_cv_.notify_all();
  }

  bool is_writer(const T& client) const {
    return has_writer_ && writer_ == client;
  }

  bool is_reader(const T& client) const { return readers_.count(client) > 0; }

  inline bool timed_acquire_write(const T& client, const zi::mutex& m,
                                  int64_t ttl) const {
    while (readers_.size() || has_writer_) {
      writer_waiting_ = true;
      if (!writer_cv_.timed_wait(m, ttl)) {
        if (readers_.size() || has_writer_) {
          writer_waiting_ = false;
          writer_cv_.notify_one();
          return false;
        }

        writer_ = client;
        has_writer_ = true;
        return true;
      }
    }
  }

  template <int64_t I>
  inline bool timed_acquire_write(
      const T& client, const zi::mutex& m,
      const zi::interval::detail::interval_tpl<I>& ttl) const {
    while (readers_.size() || has_writer_) {
      writer_waiting_ = true;
      if (!writer_cv_.wait(m, ttl)) {
        if (readers_.size() || has_writer_) {
          writer_waiting_ = false;
          writer_cv_.notify_one();
          return false;
        }

        writer_ = client;
        has_writer_ = true;
        return true;
      }
    }
  }
};

template <class ClientType, class LockType>
class rwlock_pool : zi::non_copyable {
 private:
  typedef zi::unordered_map<LockType, rwlock_impl<ClientType>*> lock_map;

  mutable zi::mutex mutex_;
  mutable lock_map locks_;

  inline rwlock_impl<ClientType>* find_or_insert_lock_nl(
      const LockType& lid) const {
    rwlock_impl<ClientType>* l;
    typename lock_map::iterator it = locks_.find(lid);

    if (it == locks_.end()) {
      l = new rwlock_impl<ClientType>;
      locks_.insert(std::make_pair(lid, l));
    } else {
      l = it->second;
    }

    return l;
  }

 public:

  rwlock_pool() : mutex_(), locks_() {}

  ~rwlock_pool() {
    for (auto kv : locks_) {
      delete kv.second;
    }
  }

  void acquire_read(const ClientType& client, const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    rwlock_impl<ClientType>* lock = find_or_insert_lock_nl(lid);

    lock->acquire_read(client, mutex_);
  }

  bool try_acquire_read(const ClientType& client, const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    rwlock_impl<ClientType>* lock = find_or_insert_lock_nl(lid);

    return lock->try_acquire_read(client);
  }

  void release_read(const ClientType& client, const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    ZI_ASSERT(locks_.count(lid) > 0);

    rwlock_impl<ClientType>* lock = find_or_insert_lock_nl(lid);
    ZI_ASSERT(lock->is_reader(client));

    lock->release_read(client);
  }

  void acquire_write(const ClientType& client, const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    rwlock_impl<ClientType>* lock = find_or_insert_lock_nl(lid);

    // ZI_ASSERT_0( lock->is_writer( client ) );
    lock->acquire_write(client, mutex_);
  }

  bool try_acquire_write(const ClientType& client, const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    rwlock_impl<ClientType>* lock = find_or_insert_lock_nl(lid);

    // ZI_ASSERT_0( lock->is_writer( client ) );
    return lock->try_acquire_write(client);
  }

  void release_write(const ClientType& client, const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    ZI_ASSERT(locks_.count(lid) > 0);

    rwlock_impl<ClientType>* lock = find_or_insert_lock_nl(lid);

    ZI_ASSERT(lock->is_writer(client));
    lock->release_write(client);
  }

  bool has_readers(const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    return find_or_insert_lock_nl(lid)->has_readers();
  }

  bool has_writers(const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    return find_or_insert_lock_nl(lid)->has_writers();
  }

  std::size_t num_readers(const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    return find_or_insert_lock_nl(lid)->num_readers();
  }

  std::size_t num_writers(const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    return find_or_insert_lock_nl(lid)->num_writers();
  }

  std::size_t num_clients(const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    return find_or_insert_lock_nl(lid)->num_clients();
  }

  bool is_writer(const ClientType& client, const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    return find_or_insert_lock_nl(lid)->is_writer(client);
  }

  bool is_reader(const ClientType& client, const LockType& lid) const {
    zi::mutex::guard g(mutex_);
    return find_or_insert_lock_nl(lid)->is_reader(client);
  }
};

}  // namespace detail
}  // namespace zi
