#ifndef STORAGE_TYPE_H
#define STORAGE_TYPE_H


#include <boost/interprocess/managed_mapped_file.hpp>



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

#endif /* STORAGE_TYPE_H */
