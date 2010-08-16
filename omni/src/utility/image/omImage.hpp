#ifndef UTILITY_IMAGE_OM_IMAGE_HPP
#define UTILITY_IMAGE_OM_IMAGE_HPP 1

#include "bits/omImage_traits.hpp"
#include "common/omCommon.h"
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>
#include <zi/utility>
#include <algorithm>
#include <functional>

template <typename T, int D>
class OmImage: zi::EnableIf<((D > 0) && (D < 10)), zi::NullType>::type {
public:
  typedef typename boost::multi_array<T, D> container_t;
  typedef          OmDimension<D>           dimension_t;

  OmImage() {};
  OmImage(OmDimension<D> extent): extent_(extent), data_() {
    data_ = boost::shared_ptr<boost::multi_array<T, D> >
      (new boost::multi_array<T, D>(extent.getBoostExtent()));
  }

  OmImage(OmDimension<D> extent, T* data): extent_(extent), data_() {
    data_ = boost::shared_ptr<boost::multi_array<T, D> >
      (new boost::multi_array<T, D>(extent.getBoostExtent()));
    data_->assign(data, data+data_->num_elements());
  }

  virtual ~OmImage() {}

  OmImage<T,D-1> getSlice(const ViewType plane, const int offset){

    assert(D == 3);
    boost::shared_ptr<boost::multi_array<T,2> > data;
    OmDimension<3> from = OmDimension<3>();
    OmDimension<3> to   = extent_;
    OmDimension<3> extent = extent_;

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

    data = boost::shared_ptr<boost::multi_array<T,2> >
      (new boost::multi_array<T,2>
       (extent.stripDimension<0>().getBoostExtent()));
    (*data) = (*data_)[MakeBoostRange<3,2>::make(from, to)];
    return OmImage<T,2>(data, extent.stripDimension<0>());

  }

  OmDimension<D> getExtent() {
    return extent_;
  }

  const T getMin() { return *std::min_element(getScalarPtr(), getScalarPtr()+size());}
  const T getMax() { return *std::max_element(getScalarPtr(), getScalarPtr()+size());}

  OmImage(boost::shared_ptr<container_t> data, OmDimension<D> extent):
    extent_(extent), data_(data) {}

  const T* getScalarPtr() {
    if (data_) {
      return data_->data();
    }
    return 0;
  }

  size_t size() {
    if (data_) {
      return data_->num_elements();
    }
    return 0;
  }

  void assign(T* data, size_t len) {
    if (data_) {
      data_->assign(data, data+len);
    }
  }

  template <typename O>
  void import(O* data, size_t len) {
    if (data_) {
      data_->assign(data, data+len);
    }
  }

  T* getMallocCopyOfData() {
    if(!data_){
      return NULL;
    }

    const int numBytes = data_->num_elements()*sizeof(T);
    T* ret = (T*)malloc(numBytes);
    memcpy(ret, data_->data(), numBytes);
    return ret;
  }

  void copyFrom(OmImage<T,D> src, OmDimension<D> targetPos,
                OmDimension<D> srcPos, OmDimension<D> size)
  {
    (*data_)[MakeBoostRange<D,D>::makeLen(targetPos, size)]
      = (*src.data_)[MakeBoostRange<D,D>::makeLen(srcPos, size)];
  }

  void setVoxel(OmDimension<D> pos, const T value) {
    boost::shared_ptr<container_t> data;
    OmImage img = OmImage<T,D>(data, OmExtents[1][1][1]);
    img.assign((T*)&value, 1);
    this->copyFrom(img, pos, OmExtents[0][0][0], OmExtents[1][1][1]);
  }

  T getVoxel(OmDimension<D> pos) {
    T ret;
    boost::shared_ptr<container_t> data;
    OmImage img = OmImage<T,D>(data, OmExtents[1][1][1]);
    img.copyFrom(*this, OmExtents[0][0][0], pos, OmExtents[1][1][1]);
    memcpy(&ret, img.data_->data(), sizeof(T));
    return ret;
  }

  void rescale(const T rangeMin, const T rangeMax, const T absMax){
        std::transform(data_->data(), data_->data()+data_->num_elements(),
    		   data_->data(), Rescale<T,T>(rangeMin, rangeMax, absMax));
  }

  template <typename C>
  OmImage<C,D> rescaleAndCast(const T rangeMin, const T rangeMax, const T absMax){
    OmImage<C,D> out(extent_);
    std::transform(data_->data(), data_->data()+data_->num_elements(),
		   out.data_->data(), Rescale<T,C>(rangeMin, rangeMax, absMax));
    return out;
  }

  template <typename C>
  OmImage<C,D> recast(){
    OmImage<C,D> out(extent_);
    std::copy(data_->begin(), data_->end(), out.data_->begin());
    return out;
  }

  template <typename T1, int D1> friend class OmImage;

private:
  OmDimension<D>                 extent_;
  boost::shared_ptr<container_t> data_;

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
