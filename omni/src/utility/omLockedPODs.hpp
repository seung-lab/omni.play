#ifndef OM_LOCKED_PODS_H
#define OM_LOCKED_PODS_H

#include <zi2/concurrency/spinlock.hpp>
#include <zi2/atomic/atomic.hpp>
#include <zi2/bits/type_traits.hpp>

/*
 * There is no need for a HEAVY RWLock here,
 * especially because *ptr = val, and  val = *ptr are
 * ATOMIC operations ( when compiled with GCC )
 */

class LockedBool
{
public:
    LockedBool(): val_( 0 )
    {
    }

    virtual ~LockedBool()
    {
    }

    bool get()
    {
        return static_cast< bool >( zi::atomic::read( &val_ ) );
    }

    void set( bool b )
    {
        zi::atomic::write( &val_, zi::atomic::atomic_word( b ) );
    }

private:
    volatile zi::atomic::atomic_word val_;

};


/*
 * Same arguments holds here. Even though RWMutex 'sounds'
 * like it should be more efficient, the actual operations
 * inside the write lock are very very fast, so the
 * condition variable stuff behind the read lock is by far
 * less efficient
 *
 * This operations are so fast that it is probably the best
 * to use spinlock ( or the adaptive pthread mutex in the worst case )
 *
 * Also, we don't want to create tons of locks( mutexes/cond variables )
 * for this type, that would just waste the system resources
 * instead we go for a mutex ( spinlock ) pool, which might have
 * collisions, but we don't care
 */

namespace Private {

struct locked_number_tag;  // used to get it's own mutex pool

template< class T > class LockedNumberDefault
{
public:
    LockedNumberDefault(): val_( static_cast< T >( 0 ) )
    {
    }

    virtual ~LockedNumberDefault()
    {
    }

    T get() const
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        return val_;
    }

    void set( const T &val )
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        val_ = val;
    }

    void add( const T &val )
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        val_ += val;
    }

    void sub( const T &val )
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        val_ -= val;
    }

    LockedNumberDefault& operator+= ( const T &val )
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        val_ += val;
        return *this;
    }

    LockedNumberDefault& operator-= ( const T &val )
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        val_ -= val;
        return *this;
    }

    LockedNumberDefault& operator= ( const T &val )
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        val_ = val;
        return *this;
    }

    LockedNumberDefault& operator++ ()
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        ++val_;
        return *this;
    }

    LockedNumberDefault& operator-- ()
    {
        zi::spinlock::pool< locked_number_tag >::guard g( this );
        --val_;
        return *this;
    }

private:
    T val_;

};

template< class T >
struct is_integral_with_lte_32_bits
{
    static const bool value =
        zi::is_integral< T >::value &&
        ( sizeof( T ) <= 4 );
};

/**
 * we can optimize even further integral numbers with
 * <= 32 bits. we could do it for 64 bit values too, but
 * i don't have that implemented yet
 *
 * Note: look at <zi2/atomic/detail/...> for some
 *       nice assembly implemented functions :)
 */

template< class T >
struct LockedIntegralNumber
{
public:
    LockedIntegralNumber()
    {
        zi::atomic::write( &val_, 0 );
    }

    virtual ~LockedIntegralNumber()
    {
    }

    T get() const
    {
        return static_cast< T >
            ( zi::atomic::read
              ( const_cast< volatile zi::atomic::atomic_word* >
                ( &val_) ) );
    }

    void set( const T &val )
    {
        zi::atomic::write( &val_, zi::atomic::atomic_word( val ) );
    }

    void add( const T &val )
    {
        (void) zi::atomic::add_swap( &val_, zi::atomic::atomic_word( val ) );
    }

    void sub( const T &val )
    {
        (void) zi::atomic::add_swap( &val_, zi::atomic::atomic_word( -val ) );
    }

    LockedIntegralNumber& operator+= ( const T &val )
    {
        (void) zi::atomic::add_swap( &val_, zi::atomic::atomic_word( val ) );
        return *this;
    }

    LockedIntegralNumber& operator-= ( const T &val )
    {
        (void) zi::atomic::add_swap( &val_, zi::atomic::atomic_word( -val ) );
        return *this;
    }

    LockedIntegralNumber& operator= ( const T &val )
    {
        zi::atomic::write( &val_, zi::atomic::atomic_word( val ) );
        return *this;
    }

    LockedIntegralNumber& operator++ ()
    {
        zi::atomic::increment( &val_ );
        return *this;
    }

    LockedIntegralNumber& operator-- ()
    {
        zi::atomic::decrement( &val_ );
        return *this;
    }

private:
    volatile zi::atomic::atomic_word val_;

};

}

template< class T, bool = Private::is_integral_with_lte_32_bits< T >::value >
struct LockedNumber {};

template< class T >
struct LockedNumber< T, false >: Private::LockedNumberDefault< T > {};

template< class T >
struct LockedNumber< T, true >: Private::LockedIntegralNumber< T > {};


typedef LockedNumber<int64_t>  LockedInt64;
typedef LockedNumber<uint64_t> LockedUint64;

typedef LockedNumber<int32_t>  LockedInt32;
typedef LockedNumber<uint32_t> LockedUint32;

#endif
