#ifndef OM_MEM_MAPPED_FILE_QT_NEW_HPP
#define OM_MEM_MAPPED_FILE_QT_NEW_HPP

#include "datalayer/fs/omIOnDiskFile.h"
#include "datalayer/fs/omFile.hpp"

template <typename T>
class OmMemMappedFileQTNew : public OmIOnDiskFile<T> {
public:
    static om::shared_ptr<OmMemMappedFileQTNew<T> >
    CreateNumElements(const std::string& fnp, const int64_t numElements)
    {
        om::file::createFileNumElements<T>(fnp, numElements);

        return om::make_shared<OmMemMappedFileQTNew<T> >(fnp);
    }

    static om::shared_ptr<OmMemMappedFileQTNew<T> >
    CreateFromData(const std::string& fnp, om::shared_ptr<T> d,
                   const int64_t numElements)
    {
        om::file::createFileFromData<T>(fnp, d, numElements);

        return om::make_shared<OmMemMappedFileQTNew<T> >(fnp);
    }

private:
    const std::string fnp_;

    boost::scoped_ptr<QFile> file_;
    T* data_;
    char* dataChar_;

    int64_t numBytes_;

public:
    OmMemMappedFileQTNew(const std::string& fnp)
        : fnp_(fnp)
        , data_(NULL)
        , dataChar_(NULL)
        , numBytes_(0)
    {
        map();
    }

    virtual ~OmMemMappedFileQTNew()
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

#endif
