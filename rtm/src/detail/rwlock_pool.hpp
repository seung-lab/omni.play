#ifndef ZI_MESHING_DETAIL_RWLOCK_POOL_HPP
#define ZI_MESHING_DETAIL_RWLOCK_POOL_HPP

#include <zi/concurrency.hpp>
#include <zi/bits/hash.hpp>
#include <zi/bits/array.hpp>
#include <zi/bits/unordered_map.hpp>

#include <zi/utility/singleton.hpp>

#include <map>

namespace zi {
namespace mesh {

struct lock_data
{
    mutable uint32_t reader_count_  ;
    mutable bool     has_writer_    ;
    mutable bool     writer_waiting_;
    mutable uint32_t ref_count_     ;

    condition_variable reader_cv_ ;
    condition_variable writer_cv_ ;

    lock_data()
        : reader_count_( 0 )
        , has_writer_( 0 )
        , writer_waiting_( 0 )
        , ref_count_( 0 )
        , reader_cv_()
        , writer_cv_()
    { }
};

template< class T >
class rwlock_pool
{
public:
    typedef T                       key_type;

private:
    mutable std::map<T, lock_data*> locks_;
    mutex                           mutex_;

    lock_data* find_lock(const T& k) const
    {
        lock_data* l;
        typename std::map<T, lock_data*>::iterator it = locks_.find(k);
        if ( it != locks_.end() )
        {
            l = it->second;
        }
        else
        {
            l = new lock_data;
            locks_[k] = l;
        }
        ++l->ref_count_;
        return l;
    }

    void return_lock(const T& k, lock_data* l) const
    {
        if ( !--l->ref_count_ )
        {
            delete l;
            locks_.erase(k);
        }
    }

public:
    rwlock_pool()
        : locks_()
        , mutex_()
    { }

    bool try_acquire_read(const T& k) const
    {
        mutex::guard g( mutex_ );
        lock_data* l = find_lock(k);

        if ( l->has_writer_ || l->writer_waiting_ )
        {
            return_lock(k, l);
            return false;
        }

        ++l->reader_count_;
        return true;
    }

    void acquire_read(const T& k) const
    {
        mutex::guard g( mutex_ );

        lock_data* l = find_lock(k);
        while ( l->has_writer_ || l->writer_waiting_ )
        {
            l->reader_cv_.wait( mutex_ );
        }

        ++l->reader_count_;
    }

    void release_read(const T& k) const
    {
        mutex::guard g( mutex_ );

        lock_data* l = find_lock(k);

        if ( !--l->reader_count_ )
        {
            l->writer_waiting_ = 0;
            l->writer_cv_.notify_one();
            l->reader_cv_.notify_all();
        }

        return_lock(k, l);
    }

    bool try_acquire_write(const T& k) const
    {
        mutex::guard g( mutex_ );

        lock_data* l = find_lock(k);

        if ( l->reader_count_ || l->has_writer_ )
        {
            return_lock(k, l);
            return false;
        }

        l->has_writer_ = true;
        return true;
    }

    void acquire_write(const T& k) const
    {
        mutex::guard g( mutex_ );
        lock_data* l = find_lock(k);

        while ( l->reader_count_ || l->has_writer_ )
        {
            l->writer_waiting_ = true;
            l->writer_cv_.wait( mutex_ );
        }

        l->has_writer_ = true;
    }

    void release_write(const T& k) const
    {
        mutex::guard g( mutex_ );
        lock_data* l = find_lock(k);

        l->has_writer_ = l->writer_waiting_ = false;
        l->writer_cv_.notify_one();
        l->reader_cv_.notify_all();
        return_lock(k,l);
    }
};

template< class T, std::size_t N, class Hash = zi::hash<T> >
class rwlock_pool_n
{
public:
    typedef T                             key_type;

private:
    zi::array< rwlock_pool<T>, N > pool_;
    Hash                           hash_;

public:
    explicit rwlock_pool_n()
        : pool_()
        , hash_()
    { }

    explicit rwlock_pool_n(const Hash& h)
        : pool_()
        , hash_(h)
    { }

    bool try_acquire_read(const T& k) const
    {
        return pool_[hash_(k)%N].try_acquire_read(k);
    }

    void acquire_read(const T& k) const
    {
        pool_[hash_(k)%N].acquire_read(k);
    }

    void release_read(const T& k) const
    {
        pool_[hash_(k)%N].release_read(k);
    }

    bool try_acquire_write(const T& k) const
    {
        return pool_[hash_(k)%N].try_acquire_write(k);
    }

    void acquire_write(const T& k) const
    {
        pool_[hash_(k)%N].acquire_write(k);
    }

    void release_write(const T& k) const
    {
        pool_[hash_(k)%N].release_write(k);
    }
};


template< class T >
class lock_pool_read_guard
{
private:
    typedef T                                       pool_type;
    typedef typename pool_type::key_type            key_type ;

    const pool_type& pool_;
    const key_type&  key_ ;

public:
    lock_pool_read_guard(const pool_type& pool, const key_type& key)
        : pool_(pool)
        , key_(key)
    {
        pool.acquire_read(key);
    }

    ~lock_pool_read_guard()
    {
        pool_.release_read(key_);
    }
};

template< class T >
class lock_pool_write_guard
{
private:
    typedef T                                       pool_type;
    typedef typename pool_type::key_type            key_type ;

    const pool_type& pool_;
    const key_type&  key_ ;

public:
    lock_pool_write_guard(const pool_type& pool, const key_type& key)
        : pool_(pool)
        , key_(key)
    {
        pool.acquire_write(key);
    }

    ~lock_pool_write_guard()
    {
        pool_.release_write(key_);
    }
};


struct dummy_static_lock_pool_guard_tag {};

template< class LockPool, class Tag = dummy_static_lock_pool_guard_tag>
class static_lock_pool_read_guard
{
private:
    typedef typename LockPool::key_type const & key_type;

private:
    key_type key_;

public:
    static_lock_pool_read_guard(typename LockPool::key_type const & key)
        : key_(key)
    {
        zi::singleton<LockPool>::template instance<Tag>().acquire_read(key);
    }

    ~static_lock_pool_read_guard()
    {
        zi::singleton<LockPool>::template instance<Tag>().release_read(key_);
    }
};

template< class LockPool, class Tag = dummy_static_lock_pool_guard_tag>
class static_lock_pool_write_guard
{
private:
    typedef typename LockPool::key_type const & key_type;

private:
    key_type key_;

public:
    static_lock_pool_write_guard(typename LockPool::key_type const & key)
        : key_(key)
    {
        zi::singleton<LockPool>::template instance<Tag>().acquire_write(key);
    }

    ~static_lock_pool_write_guard()
    {
        zi::singleton<LockPool>::template instance<Tag>().release_write(key_);
    }
};


} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_DETAIL_RWLOCK_POOL_HPP
