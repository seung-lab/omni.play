#include <iostream>
#include <map>
#include <cstddef>
#include <vector>
#include <zi/hash.hpp>
#include <zi/concurrency.hpp>
#include <zi/utility/for_each.hpp>

// get/set stuff

template< typename T >
struct storage_type
{
    std::size_t size;
    T*          data;

    storage_type( std::size_t s, T* d )
        : size(s), data(d)
    {}

    storage_type()
        : size(0), data(static_cast<T*>(0))
    {}

    storage_type( const storage_type& other )
        : size(other.size), data(other.data)
    {}

};


template< typename K, typename V >
class storage_server
{
private:
    typedef std::map< K, storage_type<V> > data_container;
    typedef typename data_container::iterator       data_iterator;
    typedef typename data_container::const_iterator data_const_iterator;

    std::map< K, storage_type<V> > data_;

public:
    storage_type<V> get( const K& key ) const
    {
        data_const_iterator it = data_.find(key);
        if ( it != data_.end() )
        {
            return it->second;
        }

        return storage_type<V>();
    }

    // return whether it got replaced
    bool set( const K& key, const storage_type<V>& store )
    {
        bool had_it = data_.find(key) != data_.end();
        data_[key] = store;
        return had_it;
    }

    bool set( const K& key, V* store, std::size_t size )
    {
        return set(key, storage_type<V>(size,store));
    }
};


template< typename K, typename V, typename H = zi::hash<K> >
class storage_client
{
private:
    typedef storage_client<K,V,H> this_type;

    static const std::size_t num_servers = 17;

    // thrift connections, or just a list of computers... or whatever
    // we (you) decide to have
    storage_server<K,V> server_[17];

    H hasher_;

    zi::task_manager::deque tm_;

    void get_in_parallel( K key, storage_type<V>* dest,
                          std::size_t* num_left, zi::mutex* m, zi::condition_variable* cv )
    {
        *dest = get(key);
        {
            zi::mutex::guard g(*m);
            --(*num_left);
            if ( *num_left == 0 )
            {
                cv->notify_one();
            }
            std::cout << "LEFT: " << *num_left << "\n";
        }
    }

public:

    storage_client()
        : server_(), hasher_(), tm_(10)
    {
        tm_.start();
    }

    ~storage_client()
    {
        tm_.join();
    }

    storage_type<V> get( const K& key ) const
    {
        std::cout << "Hashed to: " << (hasher_(key)%17) << std::endl;
        return server_[hasher_(key)%17].get(key);
    }

    std::vector< std::pair<K,storage_type<V> > >
    multi_get( const std::vector<K>& keys )
    {
        // this can also be done in parallel

        std::vector< std::pair<K,storage_type<V> > > ret(keys.size());

        std::size_t total_left = keys.size();
        zi::mutex m;
        zi::condition_variable cv;

        std::size_t i = 0;
        FOR_EACH( it, keys )
        {
            ret[i].first = *it;
            tm_.push_back( zi::bind( &this_type::get_in_parallel, this, *it, &ret[i++].second,
                                     &total_left, &m, &cv ) );
        }

        {
            zi::mutex::guard g(m);
            while ( total_left > 0 )
            {
                cv.wait(m);
            }
        }

        return ret;
    }

    // return whether it got replaced
    bool set( const K& key, const storage_type<V>& store )
    {
        return server_[hasher_(key)%17].set(key,store);
    }

    bool set( const K& key, V* store, std::size_t size )
    {
        return set(key, storage_type<V>(size,store));
    }

};



int main()
{
    storage_client<int,int> s;

    int x[1];
    int y[1];

    x[0] = 1;
    y[0] = 2;

    std::cout << s.set(1,x,1) << "\n";
    std::cout << s.set(1,x,1) << "\n";

    std::cout << s.set(2,y,1) << "\n";
    std::cout << s.set(2,y,1) << "\n";

    std::cout << (s.get(2).data)[0] << "\n";
    std::cout << (s.get(1).data)[0] << "\n";

    std::vector<int> keys;
    keys.push_back(1);
    keys.push_back(2);
    keys.push_back(1);
    keys.push_back(2);
    keys.push_back(1);
    keys.push_back(2);

    std::vector<std::pair<int,storage_type<int> > > r = s.multi_get(keys);

    FOR_EACH( it, r )
    {
        std::cout << "Key: " << it->first << " Val: " << it->second.data[0] << "\n";
    }

}
