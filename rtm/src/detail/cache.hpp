#ifndef ZI_MESHING_DETAIL_CACHE_HPP
#define ZI_MESHING_DETAIL_CACHE_HPP

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <zi/concurrency.hpp>
#include <zi/bits/shared_ptr.hpp>

#include <iostream>

namespace zi {
namespace mesh {

template< typename T >
class cache
{
private:
    typedef cache<T>             this_type;
    typedef typename T::key_type key_type;
    typedef key_type&            key_type_ref;
    typedef const key_type&      const_key_type_ref;

    typedef boost::multi_index::multi_index_container
    < T
      , boost::multi_index::indexed_by
      < boost::multi_index::sequenced<>
        , boost::multi_index::ordered_unique
        < BOOST_MULTI_INDEX_CONST_MEM_FUN(T, const_key_type_ref, key) >
        >
    > lru_list_t;

    typedef typename lru_list_t::iterator                             iterator ;
    typedef typename lru_list_t::template nth_index<1>::type          key_index;
    typedef typename key_index::iterator                              key_iterator;

    lru_list_t  list_    ;
    std::size_t max_size_;
    zi::mutex   m_       ;
    std::size_t size_    ;
    bool        downsizing_;
    zi::task_manager::simple tm_;

private:
    void shrink()
    {
        T to_remove;

        // decide whether to start removing
        bool needs_to_remove = false;
        {
            zi::mutex::guard g(m_);
            if ( size_ > max_size_ )
            {
                if ( !downsizing_ )
                {
                    downsizing_ = needs_to_remove = true;
                }
            }
        }

        while (needs_to_remove)
        {
            {
                zi::mutex::guard g(m_);
                to_remove = list_.front();
                list_.pop_front();
            }

            std::size_t s = to_remove.size();
            {
                zi::mutex::guard g(m_);
                size_ -= s;
                downsizing_ = needs_to_remove = size_ > max_size_;
                //std::cout << "--> Shrinked Cache size: " << size_ << "\n";
            }
        }
    }

    void open(T to_open)
    {
        to_open.open();
        std::size_t s = to_open.size();
        {
            zi::mutex::guard g(m_);
            size_ += s;
            //std::cout << "--> Cache size: " << size_ << "\n";
        }

        shrink();
    }

    // void touch_nl(const_key_type_ref key)
    // {
    //     // std::pair<iterator, bool> p = list_.find(key);
    //     // if ( p.second )
    //     // {
    //     //     list_.relocate(list_.end(), p.first);
    //     // }
    // }

    void erase(T to_erase)
    {
        to_erase.open();
        std::size_t s = to_erase.size();
        {
            zi::mutex::guard g(m_);
            size_ -= s;
        }
    }


public:
    explicit cache(std::size_t max_size = 1024*1024*1024)
        : list_()
        , max_size_(max_size)
        , m_()
        , size_(0)
        , downsizing_(0)
        , tm_(10)
    {
        tm_.start();
    }

    ~cache()
    {
        tm_.join();
    }

    std::size_t num_elements() const
    {
        zi::mutex::guard g(m_);
        return list_.size();
    }

    std::size_t size() const
    {
        zi::mutex::guard g(m_);
        return size_;
    }

    T get(const_key_type_ref key)
    {
        //std::cout << "--> Trying to get: " << key << '\n';

        zi::mutex::guard g(m_);
        const key_index& kindex = list_.template get<1>();
        key_iterator p = kindex.find(key);
        if ( p != kindex.end() )
        {
            list_.relocate( list_.end(), list_.template project<0>(p) );
            return *p;
        }
        else
        {
            T ret(key);
            list_.push_back(ret);
            tm_.push_back(zi::bind(&this_type::open, this, ret));
            return ret;
        }
    }

    template< typename A, typename B >
    T get( const A& a, const B& b )
    {
        key_type k(a,b);
        return get(k);
    }

    bool erase(const_key_type_ref key)
    {
        bool to_erase = false;
        T v;

        {
            zi::mutex::guard g(m_);
            key_index& kindex = list_.template get<1>();
            key_iterator p = kindex.find(key);

            if ( p != kindex.end() )
            {
                to_erase = true;
                v = (*(list_.template project<0>(p)));
                list_.erase(list_.template project<0>(p));
            }
        }

        if ( to_erase )
        {
            erase(v);
        }
        return to_erase;
    }

    bool update(const T& v)
    {
        bool to_erase = false;
        T va;

        {
            zi::mutex::guard g(m_);
            key_index& kindex = list_.template get<1>();
            key_iterator p = kindex.find(v.key());

            if ( p != kindex.end() )
            {
                to_erase = true;
                va = (*(list_.template project<0>(p)));
                list_.erase(list_.template project<0>(p));
            }

            list_.push_back(v);
            tm_.push_back(zi::bind(&this_type::open, this, v));
        }

        if ( to_erase )
        {
            erase(va);
        }

        return to_erase;
    }

    void insert(const T& v)
    {
        zi::mutex::guard g(m_);
        key_index& kindex = list_.template get<1>();
        key_index p = kindex.find(v.key());
        if ( p != kindex.end() )
        {
            tm_.push_back(zi::bind(&this_type::erase, this, *p));
            list_.erase(p);
        }
        list_.push_back(v);
        tm_.push_back(zi::bind(&this_type::open, this, v));
    }

};

template< typename K, typename V >
struct null_value_getter
{
    V operator()(const K& k) const
    {
        return V();
    }
};

template< typename V >
struct zero_value_sizer
{
    std::size_t operator()(const V& k) const
    {
        return 0;
    }
};

// template< typename Key, typename Value,
//           typename ValueGetter = null_value_getter<Key,Value>,
//           typename ValueSizer  = zero_value_sizer<Value> >
// class cached
// {
// private:
//     struct cache_node
//     {
//         struct node_control
//         {
//             bool               is_opened_ ;
//             bool               is_opening_;
//             Value              value_     ;

//             mutex              m_ ;
//             condition_variable cv_;

//             explicit node_control()
//                 : is_opened_(0)
//                 , is_opening_(0)
//                 , value_()
//                 , m_()
//                 , cv_()
//             { }

//             explicit node_control(const Value& v)
//                 : is_opened_(1)
//                 , is_opening_(1)
//                 , value_(v)
//                 , m_()
//                 , cv_()
//             { }
//         };

//         const Key                key_  ;
//         shared_ptr<node_control> data_ ;

//         cache_node()
//             : key_()
//             , data_(new node_control)
//         { }

//         cache_node(const Key& k)
//             : key_(k)
//             , data_(new node_control)
//         { }

//         cache_node(const Key& k, const Value& v)
//             : key_(k)
//             , data_(new node_control(Value))
//         { }

//         Value get_value(const Key& k, ValueGetter getter)
//         {
//             bool should_open = false;
//             bool should_wait = false;

//             {
//                 zi::mutex::guard g(m_);
//                 if ( !is_opened_ )
//                 {
//                     should_wait = true;
//                     should_open = !is_opening_;
//                 }
//             }

//             if ( should_open )
//             {
//                 value_ = getter(k);
//                 {
//                     zi::mutex::guard g(m_);
//                     is_opened_ = true;
//                     cv_.notify_all();
//                 }
//                 return value_;
//             }

//             if ( should_wait )
//             {
//                 zi::mutex::guard g(m_);
//                 if ( !is_opened_ )
//                 {
//                     cv_.wait(m_);
//                 }
//             }
//         }
//     };

// private:
//     typedef boost::multi_index::multi_index_container
//     < cache_node
//       , boost::multi_index::indexed_by
//       < boost::multi_index::sequenced<>
//         , boost::multi_index::ordered_unique
//         < BOOST_MULTI_INDEX_MEMBER(cache_node, const Key, key_) >
//         >
//     > lru_list_t;

//     typedef typename lru_list_t::iterator                             iterator ;
//     typedef typename lru_list_t::template nth_index<1>::type          key_index;
//     typedef typename key_index::iterator                              key_iterator;

//     lru_list_t  list_      ;
//     zi::mutex   mutex_     ;
//     std::size_t size_      ;
//     std::size_t max_size_  ;

//     bool        downsizing_;

//     ValueGetter getter_;
//     ValueSizer  sizer_ ;

//     zi::task_manager::simple tm_;

// public:
//     explicit cached(std::size_t max_size = 1000000000, std::size_t num_threads = 32)
//         : list_()
//         , mutex_()
//         , size_(0)
//         , max_size_(max_size)
//         , downsizing_(0)
//         , getter_()
//         , sizer_()
//         , tm_(num_threads)
//     {
//         tm_.start();
//     }

//     ~cached()
//     {
//         tm_.join();
//     }

// private:


// };


} // namespace mesh
} // namespace zi

#endif // ZI_MESHING_DETAIL_CACHE_HPP
