#pragma once

#include "container/vector.hpp"
#include "container/pagedStoragePolicy.hpp"

namespace om {
namespace data {

template <typename T>
class PagedVector : public vector<T, PagedStoragePolicy<T>> {
 public:
  PagedVector(typename PagedStoragePolicy<T>::datasource_type& pagedDS,
              size_t pageSize, size_t size)
      : vector<T, PagedStoragePolicy<T>>(PagedStoragePolicy<T>(pagedDS,
                                                               pageSize,
                                                               size)) {}
};
}
}  // namespace om::datalayer::