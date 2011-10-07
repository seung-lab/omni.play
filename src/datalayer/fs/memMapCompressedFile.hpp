#pragma once

#include "datalayer/fs/omCompressedFile.h"
#include "datalayer/fs/IOnDiskFile.h"
#include "datalayer/fs/file.h"
#include "utility/omTempFile.hpp"

template <typename T>
class memMapCompressedFile : public IOnDiskFile<T> {
public:
    static boost::shared_ptr<memMapCompressedFile<T> >
    CreateNumElements(const std::string& fnp, const int64_t numElements)
    {
        boost::shared_ptr<T> d = OmSmartPtr<T>::MallocNumElements(numElements,
                                                                  om::ZERO_FILL);
        om::file::compressToFileNumElements(d, numElements, fnp);

        return om::make_shared<memMapCompressedFile<T> >(fnp);
    }

    static boost::shared_ptr<memMapCompressedFile<T> >
    CreateFromData(const std::string& fnp, boost::shared_ptr<T> d,
                   const int64_t numElements)
    {
        om::file::compressToFileNumElements(d, numElements, fnp);

        return om::make_shared<memMapCompressedFile<T> >(fnp);
    }

private:
    const std::string fnp_;

    OmTempFile<T> tmpFile_;
    T* data_;
    char* dataChar_;

public:
    memMapCompressedFile(const std::string& fnp)
        : fnp_(fnp)
        , data_(NULL)
        , dataChar_(NULL)
    {
        uncompressAndMap();
    }

    virtual ~memMapCompressedFile()
    {}

    virtual uint64_t Size() const {
        return tmpFile_.NumBytes();
    }

    virtual void Flush()
    {
        // make compressed copy to temp file
        const std::string tmpFileName = fnp_ + ".tmp";
        om::file::compressToFileNumBytes(dataChar_, tmpFile_.NumBytes(),
                                         tmpFileName);

        // overwrite old version (don't use mvFile: temp file could be on different volume...)
        om::file::cpFile(tmpFileName, fnp_);

        om::file::rmFile(tmpFileName);
    }

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
    void uncompressAndMap()
    {
        om::file::uncompressFileToFile(fnp_, tmpFile_.FileName());

        data_ = tmpFile_.Map();

        dataChar_ = reinterpret_cast<char*>(data_);
    }
};

