#ifndef FAKE_MEM_MAP_FILE_HPP
#define FAKE_MEM_MAP_FILE_HPP

#include "utility/omSmartPtr.hpp"
#include "datalayer/fs/omIOnDiskFile.h"

template <typename T>
class FakeMemMapFile : public OmIOnDiskFile<T> {
private:
    const std::string fnp_;
    const int64_t numElements_;
    boost::shared_ptr<T> data_;

public:
    FakeMemMapFile(const std::string& fnp, const int64_t numElements)
        : fnp_(fnp)
        , numElements_(numElements)
    {
        data_ = OmSmartPtr<T>::MallocNumElements(numElements, om::ZERO_FILL);
    }

    virtual ~FakeMemMapFile()
    {}

    uint64_t Size(){
        return numElements_ * sizeof(T);
    }

    virtual T* GetPtr() const{
        return data_.get();
    }

    virtual T* GetPtrWithOffset(const int64_t offset) const
    {
        assert( 0 && offset && "fill in");
        return NULL;
    }

    virtual std::string GetBaseFileName() const {
        return "fake";
    }
};

#endif
