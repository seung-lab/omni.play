#pragma once

#include "precomp.h"
#include "utility/image_traits.hpp"
#include "common/common.h"
#include "utility/smartPtr.hpp"
#include "zi/utility.h"

namespace om {
namespace utility {

template <typename T, std::size_t D>
class imageCopiedData {
 public:
  typedef typename boost::multi_array<T, D> container_t;
  typedef dimension<D> dimension_t;

  imageCopiedData() {}

  imageCopiedData(dimension<D> extent) : extent_(extent), data_() {
    data_ =
        std::shared_ptr<container_t>(new container_t(extent.getBoostExtent()));
  }

  imageCopiedData(dimension<D> extent, T* data) : extent_(extent), data_() {
    data_ =
        std::shared_ptr<container_t>(new container_t(extent.getBoostExtent()));
    data_->assign(data, data + data_->num_elements());
  }

  imageCopiedData(std::shared_ptr<container_t> data, dimension<D> extent)
      : extent_(extent), data_(data) {}

  virtual ~imageCopiedData() {}

  dimension<D> extent_;
  std::shared_ptr<container_t> data_;
};

template <typename T, std::size_t D>
class imageRefData {
 public:
  typedef typename boost::multi_array_ref<T, D> container_t;
  typedef dimension<D> dimension_t;

  imageRefData() {}

  imageRefData(dimension<D> extent) : extent_(extent), data_() {}

  imageRefData(dimension<D> extent, T* data) : extent_(extent), data_() {
    data_ = std::shared_ptr<container_t>(
        new container_t(data, extent.getBoostExtent()));
  }

  imageRefData(std::shared_ptr<container_t> data, dimension<D> extent)
      : extent_(extent), data_(data) {}

  virtual ~imageRefData() {}

  dimension<D> extent_;
  std::shared_ptr<container_t> data_;
};

template <typename T, std::size_t D>
class imageConstRefData {
 public:
  typedef typename boost::const_multi_array_ref<T, D> container_t;
  typedef dimension<D> dimension_t;

  imageConstRefData() {}

  imageConstRefData(dimension<D> extent) : extent_(extent), data_() {}

  imageConstRefData(dimension<D> extent, T* data) : extent_(extent), data_() {
    data_ = std::shared_ptr<container_t>(
        new container_t(data, extent.getBoostExtent()));
  }

  imageConstRefData(std::shared_ptr<container_t> data, dimension<D> extent)
      : extent_(extent), data_(data) {}

  virtual ~imageConstRefData() {}

  dimension<D> extent_;
  std::shared_ptr<container_t> data_;
};

template <typename T, std::size_t D,
          template <typename DATATYPE, std::size_t DATASIZE> class container_t =
              imageCopiedData>
class image {
 public:
  image() {}

  image(dimension<D> extent) : d_(container_t<T, D>(extent)) {}

  image(dimension<D> extent, T* data) : d_(container_t<T, D>(extent, data)) {}

  image(std::shared_ptr<boost::multi_array<T, D>> data, dimension<D> extent)
      : d_(container_t<T, D>(data, extent)) {}

  virtual ~image() {}

  dimension<D> getExtent() const { return d_.extent_; }

  T const* getScalarPtr() const {
    if (d_.data_) {
      return d_.data_->data();
    }
    return 0;
  }

  T* getScalarPtrMutate() {
    if (d_.data_) {
      return d_.data_->data();
    }
    return 0;
  }

  size_t size() const {
    if (d_.data_) {
      return d_.data_->num_elements();
    }
    return 0;
  }

  void assign(T* data, size_t len) {
    if (d_.data_) {
      d_.data_->assign(data, data + len);
    }
  }

  template <typename O>
  void import(O* data, size_t len) {
    if (d_.data_) {
      d_.data_->assign(data, data + len);
    }
  }

  std::shared_ptr<T> getMallocCopyOfData() const {
    if (!d_.data_) {
      return std::shared_ptr<T>();
    }

    const int numBytes = d_.data_->num_elements() * sizeof(T);
    std::shared_ptr<T> ret = smartPtr<T>::MallocNumBytes(numBytes);
    memcpy(ret.get(), d_.data_->data(), numBytes);
    return ret;
  }

  void copyInto(void* dst) const {
    if (!d_.data_) {
      return;
    }

    const int numBytes = d_.data_->num_elements() * sizeof(T);
    memcpy(dst, d_.data_->data(), numBytes);
  }

  void copyFrom(image<T, D> src, dimension<D> targetPos, dimension<D> srcPos,
                dimension<D> size) {
    (*d_.data_)[MakeBoostRange<D, D>::makeLen(targetPos, size)] =
        (*src.d_.data_)[MakeBoostRange<D, D>::makeLen(srcPos, size)];
  }

  void resize(const Vector3i& dims) {
    d_.data_->resize(boost::extents[dims.x][dims.y][dims.z]);
  }

 private:
  container_t<T, D> d_;
};
}
}  // namespace om::utility