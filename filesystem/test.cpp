#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>
#include <functional>
#include <string>
#include <iostream>
#include <utility>
#include <cstring>

namespace bint {

using namespace boost::interprocess;



class file_storage
{
private:
    struct list_node
    {
        managed_mapped_file::handle_t key_handle      ;
        std::size_t                   size            ;
        managed_mapped_file::handle_t value_handle    ;
        managed_mapped_file::handle_t next_node_handle;
    };

    typedef managed_mapped_file::handle_t                                  handle_type        ;
    typedef std::pair<const std::size_t, handle_type>                      pair_type          ;
    typedef allocator< pair_type, managed_mapped_file::segment_manager >   file_allocator_type;
    typedef map< std::size_t, handle_type
                 , std::less<std::size_t>, file_allocator_type >           map_type           ;

    typedef map_type::iterator                                             map_iterator;
    typedef map_type::const_iterator                                       map_const_iterator;

private:
    managed_mapped_file      file_     ;
    file_allocator_type      allocator_;
    map_type*                mymap_    ;
    boost::hash<std::string> hasher_;

private:
    std::pair< const std::size_t, void* > find_in_node_list( const std::string& key,
                                                             managed_mapped_file::handle_t node )
    {
        while ( node )
        {

            list_node* n = reinterpret_cast<list_node*>(file_.get_address_from_handle(node));
            char*      k = reinterpret_cast<char*>(file_.get_address_from_handle(n->key_handle));
            if ( std::strcmp( key.c_str(), k ) == 0 )
            {
                return std::pair< const std::size_t, void* >
                    ( n->size, file_.get_address_from_handle(n->value_handle));
            }

            node = n->next_node_handle;
        }

        return std::pair< const std::size_t, void* >(0,0);
    }

    bool
    find_and_replace_node_list( const std::string& key,
                                managed_mapped_file::handle_t node,
                                std::size_t size,
                                managed_mapped_file::handle_t handle )
    {
        //managed_mapped_file::handle_t prev = node;
        list_node* prev = 0;

        while ( node )
        {
            list_node* n = reinterpret_cast<list_node*>(file_.get_address_from_handle(node));
            char*      k = reinterpret_cast<char*>(file_.get_address_from_handle(n->key_handle));
            if ( std::strcmp( key.c_str(), k ) == 0 )
            {
                file_.deallocate( file_.get_address_from_handle(n->value_handle) );
                n->size = size;
                n->value_handle = handle;
                return true;
            }

            prev = n;
            node = n->next_node_handle;
        }

        list_node* n = reinterpret_cast<list_node*>(file_.allocate(sizeof(list_node)));
        char*      k = reinterpret_cast<char*>(file_.allocate(key.size()+1));

        std::memcpy( k, key.c_str(), key.size()+1 );

        n->key_handle        = file_.get_handle_from_address(k);
        n->size              = size;
        n->value_handle      = handle;
        n->next_node_handle  = 0;

        prev->next_node_handle = file_.get_handle_from_address(n);

        return false;
    }


public:

    typedef std::pair< const std::size_t, void* >                          value_type         ;

    file_storage( const std::string& filename, std::size_t size )
        : file_( bint::open_or_create, filename.c_str(), size )
        , allocator_( file_.get_segment_manager() )
        , mymap_( file_.find_or_construct<map_type>("themap")(std::less<std::size_t>(), allocator_))
        , hasher_()
    { }

    std::size_t map_size() const
    {
        return mymap_->size();
    }

    std::size_t file_size() const
    {
        return file_.get_size();
    }

    std::size_t get_free_memory() const
    {
        return file_.get_free_memory();
    }

    std::size_t get_used_memory() const
    {
        return file_size() - get_used_memory();
    }

    std::pair< const std::size_t, void* > get( const std::string& key )
    {
        std::size_t h = hasher_(key);

        map_iterator it = mymap_->find(h);

        if ( it != mymap_->end() )
        {
            return find_in_node_list( key, it->second );
        }

        return std::pair< const std::size_t, void* >(0,0);
    }

    bool put( const std::string& key, const void* data, std::size_t size )
    {
        std::size_t h = hasher_(key);

        map_iterator it = mymap_->find(h);

        void* d = file_.allocate( size );
        managed_mapped_file::handle_t handle = file_.get_handle_from_address(d);

        std::memcpy(d, data, size);

        if ( it != mymap_->end() )
        {
            return find_and_replace_node_list( key, it->second, size, handle);
        }

        list_node* n = reinterpret_cast<list_node*>(file_.allocate(sizeof(list_node)));
        char*      k = reinterpret_cast<char*>(file_.allocate(key.size()+1));

        std::memcpy( k, key.c_str(), key.size()+1 );

        n->key_handle        = file_.get_handle_from_address(k);
        n->size              = size;
        n->value_handle      = handle;
        n->next_node_handle  = 0;

        mymap_->insert(std::pair<const std::size_t, handle_type>
                       (h, file_.get_handle_from_address(n)));

        return false;
    }

    bool put( const std::string& key, const std::string& s )
    {
        return this->put( key, s.data(), s.size() );
    }


}; // class file_storage

} // namespace bint



int main ()
{
    bint::file_storage st("some.filemap", 1024L*1024L*1024L);

    for ( int i = 0; i < 200; ++i )
    {
        std::string k = boost::lexical_cast<std::string>(i);
        std::string v = boost::lexical_cast<std::string>(i+100);
        st.put(k,v);
    }

    std::cout << "Done Storing" << "\n";

    for ( int i = 0; i < 200; ++i )
    {
        std::string k = boost::lexical_cast<std::string>(i);
        bint::file_storage::value_type v = st.get(k);
        if ( i % 10000 == 0 )
            std::cout << v.first << " " << std::string((char*)v.second,v.first) << "\n";
    }

    std::cout << "Map Size : " << st.map_size() << "\n";
    std::cout << "File Size: " << st.file_size() << "\n";
    std::cout << "Free Size: " << st.get_free_memory() << "\n";

    return 0;
}
