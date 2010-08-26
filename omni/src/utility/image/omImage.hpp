#ifndef UTILITY_IMAGE_OM_IMAGE_HPP
#define UTILITY_IMAGE_OM_IMAGE_HPP 1

#include "bits/omImage_traits.hpp"
#include "common/omCommon.h"
#include "utility/omSmartPtr.hpp"
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>
#include <zi/utility>
#include <algorithm>
#include <functional>

template <typename T, std::size_t D>
class OmImageCopiedData: zi::EnableIf<((D > 0) && (D < 10)), zi::NullType>::type {
public:
  typedef typename boost::multi_array<T, D> container_t;
  typedef          OmDimension<D>           dimension_t;

  OmImageCopiedData() {};
  OmImageCopiedData(OmDimension<D> extent): extent_(extent), data_() {
    data_ = boost::shared_ptr<container_t>
      (new container_t(extent.getBoostExtent()));
  }

 OmImageCopiedData(OmDimension<D> extent, T* data): extent_(extent), data_() {
    data_ = boost::shared_ptr<container_t>
      (new container_t(extent.getBoostExtent()));
    data_->assign(data, data+data_->num_elements());
  }

  OmImageCopiedData(boost::shared_ptr<container_t> data, OmDimension<D> extent):
    extent_(extent), data_(data) {}

  virtual ~OmImageCopiedData() {}

  OmDimension<D>                 extent_;
  boost::shared_ptr<container_t> data_;
};


template <typename T, std::size_t D>
class OmImageRefData: zi::EnableIf<((D > 0) && (D < 10)), zi::NullType>::type {
public:
  typedef typename boost::multi_array_ref<T, D> container_t;
  typedef          OmDimension<D>               dimension_t;

  OmImageRefData() {};
  OmImageRefData(OmDimension<D> extent): extent_(extent), data_() {}

  OmImageRefData(OmDimension<D> extent, T* data): extent_(extent), data_() {
    data_ = boost::shared_ptr<container_t>
	(new container_t(data, extent.getBoostExtent()));
  }

  OmImageRefData(boost::shared_ptr<container_t> data, OmDimension<D> extent):
    extent_(extent), data_(data) {}

  virtual ~OmImageRefData() {}

  OmDimension<D>                 extent_;
  boost::shared_ptr<container_t> data_;
};

template <typename T, std::size_t D>
class OmImageConstRefData: zi::EnableIf<((D > 0) && (D < 10)), zi::NullType>::type {
public:
  typedef typename boost::const_multi_array_ref<T, D> container_t;
  typedef          OmDimension<D>                     dimension_t;

  OmImageConstRefData() {};
  OmImageConstRefData(OmDimension<D> extent): extent_(extent), data_() {}

  OmImageConstRefData(OmDimension<D> extent, T* data): extent_(extent), data_() {
    data_ = boost::shared_ptr<container_t>
	(new container_t(data, extent.getBoostExtent()));
  }

 OmImageConstRefData(boost::shared_ptr<container_t> data, OmDimension<D> extent):
    extent_(extent), data_(data) {}

  virtual ~OmImageConstRefData() {}

  OmDimension<D>                 extent_;
  boost::shared_ptr<container_t> data_;
};





template <typename T, std::size_t D,
	  template <typename DATATYPE, std::size_t DATASIZE> class container_t
	  = OmImageCopiedData >
class OmImage {
public:

  OmImage() {};

  OmImage(OmDimension<D> extent)
    : d_(container_t<T,D>(extent)) {}

  OmImage(OmDimension<D> extent, T* data)
    : d_(container_t<T,D>(extent, data)) {}

  OmImage(boost::shared_ptr<boost::multi_array<T,D> > data, OmDimension<D> extent)
    : d_(container_t<T,D>(data, extent)) {}

  virtual ~OmImage() {}

  OmImage<T,2> getSlice(const ViewType plane, const int offset){

    assert(D == 3);
    OmDimension<3> from = OmDimension<3>();
    OmDimension<3> to   = d_.extent_;
    OmDimension<3> extent = d_.extent_;

    switch(plane) {

    case YZ_VIEW:
      assert(to.get<2>() > offset);
      from.set<2>(offset); to.set<2>(offset);
      break;

    case XZ_VIEW:
      assert(to.get<1>() > offset);
      from.set<1>(offset); to.set<1>(offset);
      break;

    case XY_VIEW:
      assert(to.get<0>() > offset);
      from.set<0>(offset); to.set<0>(offset);
      break;

    default:
      assert(0);
    }

    boost::shared_ptr<boost::multi_array<T,2> > data;
    data = boost::shared_ptr<boost::multi_array<T,2> >
      (new boost::multi_array<T,2>
       (extent.stripDimension<0>().getBoostExtent()));
    (*data) = (*d_.data_)[MakeBoostRange<3,2>::make(from, to)];
    return OmImage<T,2>(data, extent.stripDimension<0>());

  }

  OmDimension<D> getExtent() {
    return d_.extent_;
  }

  const T getMin() { return *std::min_element(getScalarPtr(), getScalarPtr()+size());}
  const T getMax() { return *std::max_element(getScalarPtr(), getScalarPtr()+size());}

  const T* getScalarPtr() {
    if (d_.data_) {
      return d_.data_->data();
    }
    return 0;
  }

  size_t size() {
    if (d_.data_) {
      return d_.data_->num_elements();
    }
    return 0;
  }

  void assign(T* data, size_t len) {
    if (d_.data_) {
      d_.data_->assign(data, data+len);
    }
  }

  template <typename O>
  void import(O* data, size_t len) {
    if (d_.data_) {
      d_.data_->assign(data, data+len);
    }
  }

  boost::shared_ptr<T> getMallocCopyOfData() {
    if(!d_.data_){
      return boost::shared_ptr<T>();
    }

    const int numBytes = d_.data_->num_elements()*sizeof(T);
    boost::shared_ptr<T> ret = OmSmartPtr<T>::makeMallocPtr(numBytes);
    memcpy(ret.get(), d_.data_->data(), numBytes);
    return ret;
  }

  void copyFrom(OmImage<T,D> src, OmDimension<D> targetPos,
                OmDimension<D> srcPos, OmDimension<D> size)
  {
    (*d_.data_)[MakeBoostRange<D,D>::makeLen(targetPos, size)]
      = (*src.d_.data_)[MakeBoostRange<D,D>::makeLen(srcPos, size)];
  }

  void setVoxel(const int x, const int y, const int z, const T val)
  {
    (*d_.data_)[x][y][z] = val;
  }

  T getVoxel(const int x, const int y, const int z)
  {
    return (*d_.data_)[x][y][z];
  }

  void rescale(const T rangeMin, const T rangeMax, const T absMax){
        std::transform(d_.data_->data(), d_.data_->data()+d_.data_->num_elements(),
    		   d_.data_->data(), Rescale<T,T>(rangeMin, rangeMax, absMax));
  }

  template <typename C>
  OmImage<C,D> rescaleAndCast(const T rangeMin, const T rangeMax, const T absMax){
    OmImage<C,D> out(d_.extent_);
    std::transform(d_.data_->data(), d_.data_->data()+d_.data_->num_elements(),
		   out.d_.data_->data(), Rescale<T,C>(rangeMin, rangeMax, absMax));
    return out;
  }

  template <typename C>
  OmImage<C,D> recast(){
    OmImage<C,D> out(d_.extent_);
    std::copy(d_.data_->begin(),
	      d_.data_->end(),
	      out.d_.data_->begin());
    return out;
  }

  OmImage<uint32_t,D> recastToUint32(){
    return recast<uint32_t>();
  }

  OmImage<uint8_t,D> recastToUint8(){
    return recast<uint8_t>();
  }

  template <typename T1, std::size_t D1,
	    template <typename DATATYPE1, std::size_t DATASIZE1> class container_t1>
  friend class OmImage;

private:
  container_t<T,D> d_;

  template <typename U, typename C>
  class Rescale {
  public:
    Rescale(const U rangeMin, const U rangeMax, const U absMax)
      : rangeMin_(rangeMin), rangeMax_(rangeMax), absMax_(absMax) {}
    C operator()(U val){ //FIXME: is (float) cast OK?
      return (val-rangeMin_) * ((float)absMax_/(rangeMax_-rangeMin_));
    }
  private:
    const U rangeMin_, rangeMax_, absMax_;
  };
};

#endif
