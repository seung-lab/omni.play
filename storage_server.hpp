#ifndef STORAGE_SERVER_H
#define STORAGE_SERVER_H


#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/functional/hash.hpp>
#include <iostream>
#include "storage_type.hpp"




template< typename V >
class storage_server
{

private:

    struct list_node
    {
	bint::managed_mapped_file::handle_t key_handle      ;
        std::size_t                   size            ;
	bint::managed_mapped_file::handle_t value_handle    ;
	bint::managed_mapped_file::handle_t next_node_handle;
    };



    typedef bint::managed_mapped_file::handle_t handle_t;
    typedef std::pair<const std::size_t, handle_t> pair_type;

    //Alias an STL compatible allocator of for the map.
    //This allocator will allow to place containers
    //in managed shared memory segments
    typedef bint::allocator<pair_type, bint::managed_mapped_file::segment_manager>
    m_file_allocator;

    //Alias a map of ints that uses the previous STL-like allocator.
    //Note that the third parameter argument is the ordering function
    //of the map, just like with std::map, used to compare the keys.
    typedef bint::map<std::size_t, handle_t, std::less<std::size_t>, m_file_allocator> file_map;
    typedef typename file_map::iterator       data_iterator;
    typedef typename file_map::const_iterator data_const_iterator;

    std::string name;
    std::size_t mapping_size;
    bint::managed_mapped_file mfile;
    m_file_allocator alloc_inst;
    boost::hash<std::string> hasher_;
    file_map *mymap;


    void init(){
        std::cout << "starting server...\n";

        //Construct a shared memory map.
        //Note that the first parameter is the comparison function,
        //and the second one the allocator.
        //This the same signature as std::map's constructor taking an allocator
        mymap =
            mfile.find_or_construct<file_map>(name.c_str())      //object name
            (std::less<std::size_t>() //first  ctor parameter
             ,alloc_inst);     //second ctor parameter
        std::cout << "server constructed\n";

        std::cout << "map size "<< mymap->size() << "\n";
        std::cout << "mapped memory "<< (mfile.get_size() - mfile.get_free_memory())
                  << std::endl;

    }


    storage_type<V> find_in_node_list( const std::string& key,
                                                             handle_t node ) const
    {
        while ( node )
        {

            list_node* n = reinterpret_cast<list_node*>(mfile.get_address_from_handle(node));
            char*      k = reinterpret_cast<char*>(mfile.get_address_from_handle(n->key_handle));
            if ( std::strcmp( key.c_str(), k ) == 0 )
            {
		storage_type<V> ret;
		ret.size = n->size;
		ret.data = reinterpret_cast<V*> (mfile.get_address_from_handle(n->value_handle));
                return ret;
            }

            node = n->next_node_handle;
        }

        return storage_type<V> ();
    }

    bool
    find_and_replace_node_list( const std::string& key,
                                bint::managed_mapped_file::handle_t node,
                                std::size_t size,
                                bint::managed_mapped_file::handle_t handle )
    {
        //managed_mapped_file::handle_t prev = node;
        list_node* prev = 0;

        while ( node )
        {
            list_node* n = reinterpret_cast<list_node*>(mfile.get_address_from_handle(node));
            char*      k = reinterpret_cast<char*>(mfile.get_address_from_handle(n->key_handle));
            if ( std::strcmp( key.c_str(), k ) == 0 )
            {
                mfile.deallocate( mfile.get_address_from_handle(n->value_handle) );
                n->size = size;
                n->value_handle = handle;
                return true;
            }

            prev = n;
            node = n->next_node_handle;
        }

        list_node* n = reinterpret_cast<list_node*>(mfile.allocate(sizeof(list_node)));
        char*      k = reinterpret_cast<char*>(mfile.allocate(key.size()+1));

        std::memcpy( k, key.c_str(), key.size()+1 );

        n->key_handle        = mfile.get_handle_from_address(k);
        n->size              = size;
        n->value_handle      = handle;
        n->next_node_handle  = 0;

        prev->next_node_handle = mfile.get_handle_from_address(n);

        return false;
    }



public:
    storage_server(std::string  id, std::size_t size)
        :name(id), mapping_size(size),
         mfile(bint::open_or_create ,name.append(".filemap").c_str(),mapping_size),
         alloc_inst(mfile.get_segment_manager()),
	 hasher_()
    {
        init();
    }


    ~storage_server(){
    }

    std::size_t get_size() const
    {
        return static_cast<std::size_t>(mfile.get_size());
    }

    std::size_t get_free_memory() const
    {
        return static_cast<std::size_t>(mfile.get_free_memory());
    }

    // returns the data if its mapped, empty data otherwise
    storage_type<V> get( const std::string& key ) const
    {
	std::size_t h = hasher_(key);
        data_iterator it = mymap->find(h);
	
        if ( it != mymap->end() )
        {
            return find_in_node_list( key, it->second );
        }

        return storage_type<V>();
    }

    // return whether it replaced a value
    bool set( const std::string& key, const storage_type<V>& store )
    {

	std::size_t h = hasher_(key);

        data_iterator it = mymap->find(h);

        void* d = mfile.allocate( store.size );
        handle_t handle = mfile.get_handle_from_address(d);

        std::memcpy(d, store.data, store.size);

        if ( it != mymap->end() )
        {
            return find_and_replace_node_list( key, it->second, store.size, handle);
        }

        list_node* n = reinterpret_cast<list_node*>(mfile.allocate(sizeof(list_node)));
        char*      k = reinterpret_cast<char*>(mfile.allocate(key.size()+1));

        std::memcpy( k, key.c_str(), key.size()+1 );

        n->key_handle        = mfile.get_handle_from_address(k);
        n->size              = store.size;
        n->value_handle      = handle;
        n->next_node_handle  = 0;

        mymap->insert(std::pair<const std::size_t, handle_t>
                       (h, mfile.get_handle_from_address(n)));

        return false;





    }

    bool set( const std::string& key, V* store, std::size_t size )
    {
        return set(key, storage_type<V>(size,store));
    }

    std::string get_id(){ return name;}
    std::size_t get_mapping_size(){ return mapping_size;}
};

#endif /* STORAGE_SERVER_H */
