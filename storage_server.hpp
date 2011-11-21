#ifndef STORAGE_SERVER_H
#define STORAGE_SERVER_H


#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <iostream>
#include "storage_type.hpp"




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
      (std::less<K>() //first  ctor parameter
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


    mymap->insert(*insertion);

    return had_it;
  }

  bool set( const K& key, V* store, std::size_t size )
  {
    return set(key, storage_type<V>(size,store));
  }

  std::string get_id(){ return name;}
  std::size_t get_mapping_size(){ return mapping_size;}
};

#endif /* STORAGE_SERVER_H */
