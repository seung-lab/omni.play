#ifndef OM_MEM_MAP_BACKUP_FILE_HPP
#define OM_MEM_MAP_BACKUP_FILE_HPP

#include "datalayer/fs/omCompressedFile.h"
#include "datalayer/fs/omIOnDiskFile.h"
#include "datalayer/fs/omFile.hpp"
#include "utility/omTempFile.hpp"

template <typename T>
class OmMemMapCompressedFile : public OmIOnDiskFile<T> {
public:
    static om::shared_ptr<OmMemMapCompressedFile<T> >
    CreateNumElements(const std::string& fnp, const int64_t numElements)
    {
        om::shared_ptr<T> d = OmSmartPtr<T>::MallocNumElements(numElements,
                                                                  om::ZERO_FILL);
        om::file::compressToFileNumElements(d, numElements, fnp);

        return om::make_shared<OmMemMapCompressedFile<T> >(fnp);
    }

    static om::shared_ptr<OmMemMapCompressedFile<T> >
    CreateFromData(const std::string& fnp, om::shared_ptr<T> d,
                   const int64_t numElements)
    {
        om::file::compressToFileNumElements(d, numElements, fnp);

        return om::make_shared<OmMemMapCompressedFile<T> >(fnp);
    }

private:
    const std::string fnp_;

    OmTempFile<T> tmpFile_;
    T* data_;
    char* dataChar_;

public:
    OmMemMapCompressedFile(const std::string& fnp)
        : fnp_(fnp)
        , data_(NULL)
        , dataChar_(NULL)
    {
        uncompressAndMap();
    }

    virtual ~OmMemMapCompressedFile()
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

#endif
