#pragma once

#include "datalayer/fs/IOnDiskFile.h"
#include "datalayer/fs/file.h"

template <typename T>
class memMappedFileQTNew : public IOnDiskFile<T> {
public:
    static boost::shared_ptr<memMappedFileQTNew<T> >
    CreateNumElements(const std::string& fnp, const int64_t numElements)
    {
        om::file::createFileNumElements<T>(fnp, numElements);

        return om::make_shared<memMappedFileQTNew<T> >(fnp);
    }

    static boost::shared_ptr<memMappedFileQTNew<T> >
    CreateFromData(const std::string& fnp, boost::shared_ptr<T> d,
                   const int64_t numElements)
    {
        om::file::createFileFromData<T>(fnp, d, numElements);

        return om::make_shared<memMappedFileQTNew<T> >(fnp);
    }

private:
    const std::string fnp_;

    boost::scoped_ptr<QFile> file_;
    T* data_;
    char* dataChar_;

    int64_t numBytes_;

public:
    memMappedFileQTNew(const std::string& fnp)
        : fnp_(fnp)
        , data_(NULL)
        , dataChar_(NULL)
        , numBytes_(0)
    {
        map();
    }

    virtual ~memMappedFileQTNew()
    {}

    virtual uint64_t Size() const {
        return numBytes_;
    }

    virtual void Flush()
    {}

    virtual T* GetPtr() const {
        return data_;
    }

    virtual T* GetPtrWithOffset(const int64_t offset) const {
        return reinterpret_cast<T*>( dataChar_ + offset );
    }

    virtual std::string GetBaseFileName() const {
        return fnp_;
    }

private:
    void map()
    {
        om::file::openFileRW(file_, fnp_);
        data_ = om::file::mapFile<T>(file_);
        dataChar_ = reinterpret_cast<char*>(data_);
    }
};

