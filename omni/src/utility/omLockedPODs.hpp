#pragma once

#include <zi/atomic/atomic.hpp>
#include <zi/bits/type_traits.hpp>
#include <zi/concurrency/guard.hpp>
#include <zi/concurrency/spinlock.hpp>

class LockedBool {
public:
    LockedBool(): val_( 0 )
    {}

    virtual ~LockedBool()
    {}

    bool get(){
        return static_cast< bool >( zi::atomic::read( &val_ ) );
    }

    void set(const bool b){
        zi::atomic::write( &val_, zi::atomic::atomic_word( b ) );
    }

private:
    volatile zi::atomic::atomic_word val_;
};


/*
 * Operations on val_ are so fast that it is probably the best
 * to use spinlock ( or the adaptive pthread mutex in the worst case )
 *
 * Also, we don't want to create tons of locks( mutexes/cond variables )
 * for this type, that would just waste the system resources
 * instead we go for a mutex ( spinlock ) pool, which might have
 * collisions, but we don't care
 */

namespace om {
namespace locked_pods_ {

struct locked_number_tag;  // used to get it's own mutex pool

template< class T > class LockedNumberDefault
{
public:
    LockedNumberDefault(): val_( static_cast< T >( 0 ) )
    {}

    virtual ~LockedNumberDefault()
    {}

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
 * Note: look at <external/zi_lib/zi/atomic/detail/...> for some
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
    {}

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

    T inc()
    {
        return static_cast< T >( zi::atomic::increment_swap( &val_ ) ) + 1;
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

} // namespace locked_pods_
} // namespace om

template< class T, bool = om::locked_pods_::is_integral_with_lte_32_bits< T >::value >
struct LockedNumber {};

template< class T >
struct LockedNumber< T, false > : om::locked_pods_::LockedNumberDefault< T > {};

template< class T >
struct LockedNumber< T, true > : om::locked_pods_::LockedIntegralNumber< T > {};

typedef LockedNumber<int64_t>  LockedInt64;
typedef LockedNumber<uint64_t> LockedUint64;

typedef LockedNumber<int32_t>  LockedInt32;
typedef LockedNumber<uint32_t> LockedUint32;

template <typename T>
class OmLockedNumber {
private:
    T val_;

public:
    OmLockedNumber()
        : val_(0)
    {}

    OmLockedNumber(const T val)
        : val_(val)
    {}

    ~OmLockedNumber()
    {}

    T get() const
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        return val_;
    }

    void set(const T val)
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        val_ = val;
    }

    T inc()
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        return ++val_;
    }

    OmLockedNumber& operator+= (const T val)
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        val_ += val;
        return *this;
    }

    OmLockedNumber& operator-= (const T val)
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        val_ -= val;
        return *this;
    }

    OmLockedNumber& operator= (const T val)
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        val_ = val;
        return *this;
    }

    OmLockedNumber& operator++ ()
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        ++val_;
        return *this;
    }

    OmLockedNumber& operator-- ()
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        --val_;
        return *this;
    }

    OmLockedNumber operator++ (int)
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        OmLockedNumber<T> ret(val_);
        ++val_;
        return ret;
    }

    OmLockedNumber operator-- (int)
    {
        zi::spinlock::pool<om::locked_pods_::locked_number_tag>::guard g(this);
        OmLockedNumber<T> ret(val_);
        --val_;
        return ret;
    }
};

typedef OmLockedNumber<int32_t>  OmLockedInt32;
typedef OmLockedNumber<uint32_t> OmLockedUint32;

typedef OmLockedNumber<int64_t>  OmLockedInt64;
typedef OmLockedNumber<uint64_t> OmLockedUint64;

