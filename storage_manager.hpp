#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <vector>
#include <zi/hash.hpp>
#include <sstream>
#include "storage_server.hpp"



template< typename K, typename V, typename H = zi::hash<K> >
class storage_manager
{
private:
  static const std::size_t num_servers = 5;
  static const std::size_t file_mapping_size = 50*1024; //in megabytes
  // thrift connections, or just a list of computers... or whatever
  // we (you) decide to have
  std::vector< storage_server<K,V>*  > server_;
  
  H hasher_;

public:
  storage_manager()
    :server_(),hasher_()
  {
    //construct servers
    unsigned int i;
    for(i=0;i<num_servers;i++)
      {
	std::stringstream ss;
	ss << i << "-filemap";
	storage_server<K,V>* p = new storage_server<K,V>
	  (ss.str(),file_mapping_size ); 
	server_.push_back(p);
      }

  }
  ~storage_manager()
  {
    //deallocate servers
    unsigned int i;
    for(i=0;i<num_servers;i++)
      {
	delete server_[i];
      }
    
  }
  
  storage_server<K,V>* get_server(const K& key){
    //for now just do the hash based on the fixed number of servers
    //later do better load-balancing
    return server_[hasher_(key) % num_servers];
  }


};

#endif /* STORAGE_MANAGER_H */
