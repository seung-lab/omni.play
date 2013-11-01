#pragma once
#ifndef ZI_RWLOCK_SERVER_HPP
#define ZI_RWLOCK_SERVER_HPP

#include "rwlock_pool.hpp"

#include <zi/unordered_map.hpp>
#include <zi/array.hpp>
#include <zi/bits/hash.hpp>

namespace zi {

template <class ClientType, class LockType, std::size_t NumBuckets>
class rwlock_server {
 private:
  static const std::size_t num_buckets = NumBuckets;

  typedef ClientType client_type;
  typedef LockType lock_type;

  typedef hash<lock_type> hasher_type;

 protected:
  zi::array<detail::rwlock_pool<client_type, lock_type>, num_buckets> pools_;
  hasher_type hasher_;

 public:
  rwlock_server() : pools_(), hasher_() {}

  ~rwlock_server() {}

  bool acquire_read(const client_type& clt, const lock_type& lid) {
    pools_[hasher_(lid) % num_buckets].acquire_read(clt, lid);
    return true;
  }

  bool try_acquire_read(const client_type& clt, const lock_type& lid) {
    return pools_[hasher_(lid) % num_buckets].try_acquire_read(clt, lid);
  }

  bool try_acquire_write(const client_type& clt, const lock_type& lid) {
    return pools_[hasher_(lid) % num_buckets].try_acquire_write(clt, lid);
  }

  bool acquire_write(const client_type& clt, const lock_type& lid) {
    pools_[hasher_(lid) % num_buckets].acquire_write(clt, lid);
    return true;
  }

  bool release_read(const client_type& clt, const lock_type& lid) {
    pools_[hasher_(lid) % num_buckets].release_read(clt, lid);
    return true;
    ;
  }

  bool release_write(const client_type& clt, const lock_type& lid) {
    pools_[hasher_(lid) % num_buckets].release_write(clt, lid);
    return true;
    ;
  }

  bool acquire(const client_type& clt, const lock_type& lid) {
    pools_[hasher_(lid) % num_buckets].acquire_write(clt, lid);
    return true;
    ;
  }

  bool release(const client_type& clt, const lock_type& lid) {
    pools_[hasher_(lid) % num_buckets].release_write(clt, lid);
    return true;
    ;
  }
};

}  // namespace zi

#endif
