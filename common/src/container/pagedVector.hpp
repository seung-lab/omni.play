#pragma once

#include "container/vector.hpp"
#include "container/dynarray.hpp"
#include "container/pagedStoragePolicy.hpp"

namespace om {
namespace data {

template <typename T>
class PagedVector : public vector<T, PagedStoragePolicy<T>> {
 public:
  PagedVector(typename PagedStoragePolicy<T>::datasource_type& pagedDS,
              size_t pageSize, size_t size)
      : vector<T, PagedStoragePolicy<T>>(
            PagedStoragePolicy<T>(pagedDS, pageSize, size)) {}
};

#define DEFAULT_MEM_PAGE_SIZE 100

template <typename T>
using paged_dynarray = typename data::dynarray<
    T, data::MemPagedStoragePolicy<T, DEFAULT_MEM_PAGE_SIZE>>;
}
}  // namespace om::datalayer::