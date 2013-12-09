#pragma once

#include "datalayer/dataSource.hpp"
#include "datalayer/vector.hpp"

namespace om {
namespace data {

template <typename T> class PagedStoragePolicy {
 public:
  typedef size_t index_type;
  typedef datalayer::Vector<index_type, T> page_type;
  typedef std::shared_ptr<datalayer::Vector<index_type, T>> page_ptr_type;
  typedef datalayer::IDataSource<index_type, page_type> datasource_type;

  PagedStoragePolicy(datasource_type& pagedDS, index_type pageSize,
                     index_type size)
      : pagedDS_(pagedDS),
        pageSize_(pageSize),
        size_(size),
        backendSize_(size),
        pages_(numPages()) {}

  index_type size() const { return size_; }

  void resize(index_type n, const T& val) {
    size_ = n;
    pages_.resize(numPages());
    for (int page = 0; page < numPages(); ++page) {
      auto newPageSize =
          (page == numPages() - 1) ? size_ % pageSize_ : pageSize_;
      if (newPageSize != pages_[page]->Values.size()) {
        pages_[page]->Values.resize(newPageSize, val);
      }
    }
  }

  T& doGet(index_type i) {
    auto page = getPage(i);
    return page->Values[i % pageSize_];
  }

  const T& doGet(index_type i) const {
    auto page = getPage(i);
    return page->Values[i % pageSize_];
  }

  void flush() {
    // Push Old & New
    for (index_type i = 0; i < numPages(); ++i) {
      if (pages_[i]) {
        // TODO: Check for failure?
        pagedDS_.Put(i, pages_[i]);
      }
    }

    // Push Deletes
    for (index_type i = numPages(); i < numBackendPages(); ++i) {
      if (pages_[i]) {
        // TODO: Check for failure?
        pagedDS_.Put(i, std::make_shared<page_type>(i));
      }
    }

    backendSize_ = size_;
  }

 private:
  page_ptr_type getPage(index_type idx) const {
    auto pageNum = idx / pageSize_;
    if (!pages_[pageNum]) {
      pages_[pageNum] = pagedDS_.Get(pageNum);
    }
    if (!pages_[pageNum]) {
      pages_[pageNum] = std::make_shared<page_type>(pageNum);
    }
    return pages_[pageNum];
  }

  index_type numPages() const { return (size_ / pageSize_) + 1; }
  index_type numBackendPages() const { return (backendSize_ / pageSize_) + 1; }

  datasource_type& pagedDS_;
  index_type pageSize_;
  index_type size_;
  index_type backendSize_;
  mutable std::vector<page_ptr_type> pages_;
};
}
}  // namespace om::datalayer::
