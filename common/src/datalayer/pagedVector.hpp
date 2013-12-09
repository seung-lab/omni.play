#pragma once

#include "container/vector.hpp"
#include "container/pagedStoragePolicy.hpp"

namespace om {
namespace datalayer {

template <typename T>
class PagedVector : public data::vector<T, data::PagedStoragePolicy<T>> {
  typedef data::vector<T, data::PagedStoragePolicy<T>> base_t;

 public:
  PagedVector(IDataSource<size_t, T>& pagedDS, size_t pageSize, size_t size)
      : base_t(PagedStoragePolicy(pagedDS, pageSize, size)) {}
};
}
}  // namespace om::datalayer::