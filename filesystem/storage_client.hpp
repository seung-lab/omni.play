#ifndef STORAGE_CLIENT_H
#define STORAGE_CLIENT_H

#include <iostream>
#include <vector>
#include <zi/concurrency.hpp>
#include <zi/utility/for_each.hpp>
#include "storage_manager.hpp"


template< typename K, typename V >
class storage_client
{
private:
  typedef storage_client<K,V > this_type;
  
  zi::task_manager::deque tm_;
  storage_manager<K,V> manager_;


  void get_in_parallel( K key, storage_type<V>* dest,
			std::size_t* num_left
			, zi::mutex* m
			, zi::condition_variable* cv )
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
    : tm_(10),manager_()
  {
    std::cout << "starting client\n";
    tm_.start();
    std::cout << "client started\n";
  }

  ~storage_client()
  {
    tm_.join();
  }

  storage_type<V> get( const K& key ) //const
  {
    storage_server<V>* server = manager_.get_server(key);
    
    return server->get(key);
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
    storage_server<V>* server = manager_.get_server(key);
    return server->set(key,store);
  }

  bool set( const K& key, V* store, std::size_t size )
  {
    return set(key, storage_type<V>(size,store));
  }

};

#endif /* STORAGE_CLIENT_H */
