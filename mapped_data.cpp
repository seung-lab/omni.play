#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <iostream>
#include <sstream>
#include <cstddef>
#include <vector>
#include <zi/hash.hpp>
#include <zi/concurrency.hpp>
#include <zi/utility/for_each.hpp>

// get/set stuff
namespace bint{
using namespace boost::interprocess;
}

template< typename T >
struct storage_type
{
  std::size_t size;
  T*          data;
  bint::managed_mapped_file::handle_t handle;

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
    
  typedef bint::managed_mapped_file::handle_t handle_t;
  typedef std::pair<K,  handle_t> value_type;
  
  //Alias an STL compatible allocator of for the map.
  //This allocator will allow to place containers
  //in managed shared memory segments
  typedef bint::allocator<value_type, bint::managed_mapped_file::segment_manager> 
  m_file_allocator;

  //Alias a map of ints that uses the previous STL-like allocator.
  //Note that the third parameter argument is the ordering function
  //of the map, just like with std::map, used to compare the keys.
  typedef bint::map<K, handle_t, std::less<K>, m_file_allocator> file_map;
  typedef typename file_map::iterator       data_iterator;
  typedef typename file_map::const_iterator data_const_iterator;

  std::string name;
  std::size_t mapping_size;
  bint::managed_mapped_file mfile;
  m_file_allocator alloc_inst;
  file_map *mymap;

  void init(){
    std::cout << "starting server...\n";
    
    //Construct a shared memory map.
    //Note that the first parameter is the comparison function,
    //and the second one the allocator.
    //This the same signature as std::map's constructor taking an allocator
    mymap = 
      mfile.find_or_construct<file_map>("FileMap")      //object name
      (std::less<int>() //first  ctor parameter
       ,alloc_inst);     //second ctor parameter
    std::cout << "server constructed\n";
    
    std::cout << "map size "<< mymap->size() << "\n";
    std::cout << "mapped memory "<< (mfile.get_size() - mfile.get_free_memory())
	      << std::endl;

  }

public:
  storage_server(std::string  id, std::size_t size)
    :name(id), mapping_size(size), 
     mfile(bint::open_or_create ,name.c_str(),mapping_size), 
     alloc_inst(mfile.get_segment_manager())
  {
    init();
  }

    
  ~storage_server(){
   }

  // returns the data if its mapped, empty data otherwise
  storage_type<V> get( const K& key ) const
  {
    data_const_iterator it = mymap->find(key);
    
    if ( it != mymap->end() )
      {
	//get handle and convert it back into storage_type
	handle_t handle = it->second;
	storage_type<V>* store = static_cast<storage_type<V>* >
	  (mfile.get_address_from_handle(handle));
	store->data = reinterpret_cast<V*>(mfile.get_address_from_handle(store->handle));
	return *store;
      }
    //return empty data on failure
    return storage_type<V>();
  }

  // return whether it got replaced
  bool set( const K& key, const storage_type<V>& store )
  {
    //construct value_type in memory-mapped region
    void* new_data = mfile.allocate(sizeof(V)*store.size);
    std::memcpy(new_data,store.data,sizeof(V)*store.size);
    
    storage_type<V>* new_store = mfile.construct<storage_type<V> >
      (bint::anonymous_instance)(
    				 store.size
				 ,static_cast<V*>(new_data));
    //store handle for data ptr
    new_store->handle = mfile.get_handle_from_address(new_data);

    //get handle for allocated storage_type
    handle_t* handle = mfile.construct<handle_t>
      (bint::anonymous_instance)(
				 mfile.get_handle_from_address(new_store));

    value_type* insertion = mfile.construct<value_type>
      (bint::anonymous_instance)(key,*handle);
    

    data_const_iterator it = mymap->find(key);
    bool had_it = mymap->find(key) != mymap->end();
    //if had_it, we have to erase the old value to replace
    if(had_it)
      {
	data_const_iterator it_delete = mymap->find(key);
    
	
	//get handle and convert it back into storage_type
	handle_t handle_delete = it_delete->second;
	storage_type<V>* store_delete = static_cast<storage_type<V>* >
	  (mfile.get_address_from_handle(handle_delete));

	store_delete->data = reinterpret_cast<V*>(mfile.get_address_from_handle(store_delete->handle));
	//TODO deallocate memory
	mfile.deallocate(store_delete->data);
	//destroy object
	mfile.destroy_ptr(store_delete);	
	//erase from map
	mymap->erase(key);
	
      }

    std::cout << "inserting key-value pair\n";
    mymap->insert(*insertion);
    std::cout << "inserted.\n";
    return had_it;
  }

  bool set( const K& key, V* store, std::size_t size )
  {
    return set(key, storage_type<V>(size,store));
  }

  std::string get_id(){ return name;}
  std::size_t get_mapping_size(){ return mapping_size;}
};


template< typename K, typename V, typename H = zi::hash<K> >
class storage_manager
{
private:
  static const std::size_t num_servers = 5;
  static const std::size_t file_mapping_size = 20*1024*1024; //in megabytes
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
    storage_server<K,V>* server = manager_.get_server(key);
    std::cout << "Stored in: " << server->get_id() << std::endl;
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
    storage_server<K,V>* server = manager_.get_server(key);
    return server->set(key,store);
  }

  bool set( const K& key, V* store, std::size_t size )
  {
    return set(key, storage_type<V>(size,store));
  }

};



int main()
{
  storage_client<int,int> s;

  // int x[1];
  // int y[1];

  // x[0] = 1;
  // y[0] = 2;

  // std::cout << s.set(1,x,1) << "\n";
  // std::cout << s.set(1,x,1) << "\n";

  // std::cout << s.set(2,y,1) << "\n";
  // std::cout << s.set(2,y,1) << "\n";

  // std::cout << (s.get(2).data)[0] << "\n";
  // std::cout << (s.get(1).data)[0] << "\n";

  // std::vector<int> keys;
  // keys.push_back(1);
  // keys.push_back(2);
  // keys.push_back(1);
  // keys.push_back(2);
  // keys.push_back(1);
  // keys.push_back(2);
  
  int size = 100;
  int i;
  std::vector<int> keys;
  for(i = 0;i<size;i++)
    {
      int x[2] =  {2,1};
      s.set(i,x,2);
      keys.push_back(i);
      std::cout << "here\n";
    }
  

  


  std::vector<std::pair<int,storage_type<int> > > r = s.multi_get(keys);

  FOR_EACH( it, r )
    {
      std::cout << "Key: " << it->first << " Val: " << it->second.data[0] << "\n";
    }
  //remove mapped file
  //bint::file_mapping::remove("MappedFile");
}
