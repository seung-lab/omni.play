#pragma once

#include "datalayer/dataSource.hpp"
#include "datalayer/vector.hpp"
#include "utility/malloc.hpp"

namespace om {
namespace data {

template <typename T>
class PagedStoragePolicy {
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
        pages_(numPages()) {
  }

  index_type size() const { return size_; }

  void resize(const index_type n, const T& val) {
    size_ = n;
    pages_.resize(numPages());

    for (int page = 0; page < numPages(); ++page) {
      if(!pages_[page] ){
        pages_[page].reset(new page_type(page));
        pages_[page]->Values.resize(pageSize_,val);
      }
    }
  }

  void resize(const index_type n) {
    size_ = n;
    pages_.resize(numPages());

    for (int page = 0; page < numPages(); ++page) {
      if(!pages_[page]){
        pages_[page].reset(new page_type(page));
        pages_[page]->Values.resize(pageSize_);
      }
    }
  }


  void reserve(index_type n) {
    auto reservePages = (n / pageSize_) + 1;
    pages_.resize(reservePages);
    for (int page = 0; page < reservePages; ++page) {
      auto newPageSize =
          (page == reservePages - 1) ? size_ % pageSize_ : pageSize_;
      pages_[page]->Values.reserve(newPageSize);
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
        if (!pagedDS_.Put(i, pages_[i])) {
          log_errors << "Unable to save page " << i;
        }
      }
    }

    // Push Deletes
    for (index_type i = numPages(); i < numBackendPages(); ++i) {
      if (pages_[i]) {
        if (!pagedDS_.Put(i, std::make_shared<page_type>(i))) {
          log_errors << "Unable to delete page " << i;
        }
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
      throw IoException("Unable to get paged data.");
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

template <typename T, size_t PageSize, typename Enable = void>
class MemPagedStoragePolicy {
 public:
  typedef size_t index_type;
  typedef std::array<T, PageSize> page_type;
  typedef std::unique_ptr<page_type> page_ptr_type;

  MemPagedStoragePolicy(index_type size = 0)
      : size_(size), pages_(numPages()) {}

  index_type size() const { return size_; }

  void resize(index_type n, const T& val) {
    size_ = n;
    pages_.resize(numPages());
  }

  void resize(index_type n) {
    size_ = n;
    pages_.resize(numPages());
  }

  void reserve(index_type n) {
    auto reservePages = (n / PageSize) + 1;
    pages_.resize(reservePages);
  }

  T& doGet(index_type i) {
    auto page = getPage(i);
    return (*page)[i % PageSize];
  }

  const T& doGet(index_type i) const {
    auto page = getPage(i);
    return (*page)[i % PageSize];
  }

  void flush() {}

 private:
  page_ptr_type getPage(index_type idx) const {
    auto pageNum = idx / PageSize;
    if (!pages_[pageNum]) {
      pages_[pageNum] = std::make_unique<page_type>();
    }
    return pages_[pageNum];
  }

  index_type numPages() const { return (size_ / PageSize) + 1; }

  index_type size_;
  mutable std::vector<page_ptr_type> pages_;
};

template <typename T, size_t PageSize>
class MemPagedStoragePolicy<
    T, PageSize,
    typename std::enable_if<!std::is_default_constructible<T>::value>::type> {
 public:
  typedef size_t index_type;
  typedef T* page_type;
  typedef std::shared_ptr<T> page_ptr_type;

  MemPagedStoragePolicy(index_type size = 0)
      : size_(size), pages_(numPages()) {}

  index_type size() const { return size_; }

  void resize(index_type n, const T& val) {
    size_ = n;
    pages_.resize(numPages());
  }

  void resize(index_type n) {
    size_ = n;
    pages_.resize(numPages());
  }

  void reserve(index_type n) {
    auto reservePages = (n / PageSize) + 1;
    pages_.resize(reservePages);
  }

  T& doGet(index_type i) {
    page_type page = getPage(i);
    return page[i % PageSize];
  }

  const T& doGet(index_type i) const {
    page_type page = getPage(i);
    return page[i % PageSize];
  }

  void flush() {}

 private:
  page_type getPage(index_type idx) const {
    auto pageNum = idx / PageSize;
    if (!pages_[pageNum]) {
      pages_[pageNum] = om::mem::Malloc<T>::NumElementsDontZero(PageSize);
    }
    return pages_[pageNum].get();
  }

  index_type numPages() const { return (size_ / PageSize) + 1; }

  index_type size_;
  mutable std::vector<page_ptr_type> pages_;
};
}
}  // namespace om::datalayer::
