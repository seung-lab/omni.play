#pragma once

#include "bits/omImage_traits.hpp"
#include "common/common.h"
#include "utility/malloc.hpp"
#include "zi/omUtility.h"

#include <algorithm>
#include <functional>
#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

template <typename T, std::size_t D>
class OmImageCopiedData {
public:
    typedef typename boost::multi_array<T, D> container_t;
    typedef          OmDimension<D>           dimension_t;

    OmImageCopiedData()
    {}

    OmImageCopiedData(OmDimension<D> extent)
        : extent_(extent)
        , data_()
    {
        data_ = std::shared_ptr<container_t>
            (new container_t(extent.getBoostExtent()));
    }

    OmImageCopiedData(OmDimension<D> extent, T* data)
        : extent_(extent)
        , data_()
    {
        data_ = std::shared_ptr<container_t>
            (new container_t(extent.getBoostExtent()));
        data_->assign(data, data+data_->num_elements());
    }

    OmImageCopiedData(std::shared_ptr<container_t> data, OmDimension<D> extent)
        : extent_(extent)
        , data_(data)
    {}

    virtual ~OmImageCopiedData()
    {}

    OmDimension<D>                 extent_;
    std::shared_ptr<container_t> data_;
};

template <typename T, std::size_t D>
class OmImageRefData {
public:
    typedef typename boost::multi_array_ref<T, D> container_t;
    typedef          OmDimension<D>               dimension_t;

    OmImageRefData()
    {}

    OmImageRefData(OmDimension<D> extent)
        : extent_(extent)
        , data_()
    {}

    OmImageRefData(OmDimension<D> extent, T* data)
        : extent_(extent)
        , data_()
    {
        data_ = std::shared_ptr<container_t>
            (new container_t(data, extent.getBoostExtent()));
    }

    OmImageRefData(std::shared_ptr<container_t> data, OmDimension<D> extent)
        : extent_(extent)
        , data_(data)
    {}

    virtual ~OmImageRefData()
    {}

    OmDimension<D>                 extent_;
    std::shared_ptr<container_t> data_;
};

template <typename T, std::size_t D>
class OmImageConstRefData {
public:
    typedef typename boost::const_multi_array_ref<T, D> container_t;
    typedef          OmDimension<D>                     dimension_t;

    OmImageConstRefData()
    {}

    OmImageConstRefData(OmDimension<D> extent)
        : extent_(extent)
        , data_()
    {}

    OmImageConstRefData(OmDimension<D> extent, T* data)
        : extent_(extent)
        , data_()
    {
        data_ = std::shared_ptr<container_t>
            (new container_t(data, extent.getBoostExtent()));
    }

    OmImageConstRefData(std::shared_ptr<container_t> data, OmDimension<D> extent)
        : extent_(extent)
        , data_(data)
    {}

    virtual ~OmImageConstRefData()
    {}

    OmDimension<D>                 extent_;
    std::shared_ptr<container_t> data_;
};

template <typename T, std::size_t D,
          template <typename DATATYPE, std::size_t DATASIZE> class container_t
          = OmImageCopiedData >
class OmImage {
public:

    OmImage()
    {}

    OmImage(OmDimension<D> extent)
        : d_(container_t<T,D>(extent))
    {}

    OmImage(OmDimension<D> extent, T* data)
        : d_(container_t<T,D>(extent, data))
    {}

    OmImage(std::shared_ptr<boost::multi_array<T,D> > data, OmDimension<D> extent)
        : d_(container_t<T,D>(data, extent))
    {}

    virtual ~OmImage()
    {}

    OmDimension<D> getExtent() const {
        return d_.extent_;
    }

    T const* getScalarPtr() const
    {
        if(d_.data_) {
            return d_.data_->data();
        }
        return 0;
    }

    T* getScalarPtrMutate()
    {
        if(d_.data_) {
            return d_.data_->data();
        }
        return 0;
    }

    size_t size() const
    {
        if(d_.data_) {
            return d_.data_->num_elements();
        }
        return 0;
    }

    void assign(T* data, size_t len)
    {
        if (d_.data_) {
            d_.data_->assign(data, data+len);
        }
    }

    template <typename O>
    void import(O* data, size_t len)
    {
        if (d_.data_) {
            d_.data_->assign(data, data+len);
        }
    }

    std::shared_ptr<T> getMallocCopyOfData() const
    {
        if(!d_.data_){
            return std::shared_ptr<T>();
        }

        const int numBytes = d_.data_->num_elements()*sizeof(T);
        std::shared_ptr<T> ret =
        om::mem::Malloc<T>::NumBytes(numBytes, om::mem::ZeroFill::DONT);
        memcpy(ret.get(), d_.data_->data(), numBytes);
        return ret;
    }

    void copyInto(void* dst) const
    {
        if(!d_.data_){
            return;
        }

        const int numBytes = d_.data_->num_elements()*sizeof(T);
        memcpy(dst, d_.data_->data(), numBytes);
    }

    void copyFrom(OmImage<T,D> src, OmDimension<D> targetPos,
                  OmDimension<D> srcPos, OmDimension<D> size)
    {
        (*d_.data_)[MakeBoostRange<D,D>::makeLen(targetPos, size)]
            = (*src.d_.data_)[MakeBoostRange<D,D>::makeLen(srcPos, size)];
    }

    void resize(const Vector3i& dims) {
        d_.data_->resize(boost::extents[dims.x][dims.y][dims.z]);
    }

    template <typename T1, std::size_t D1,
              template <typename DATATYPE1, std::size_t DATASIZE1> class container_t1>

    friend class OmImage;

private:
    container_t<T,D> d_;
};

