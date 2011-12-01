#pragma once

#include "boost/iostreams/device/mapped_file.hpp"
#include "boost/filesystem.hpp"
//#include "boost/format.hpp"
#include "boost/make_shared.hpp"

#include "datalayer/IOnDiskFile.h"
#include "datalayer/file.h"


typedef boost::iostreams::mapped_file mapped_file;


namespace om {
namespace datalayer {

template <typename T>
class memMappedFile : public IOnDiskFile<T> {
public:
    static boost::shared_ptr<memMappedFile<T> >
    CreateNumElements(const std::string& fnp, const int64_t numElements)
    {
        om::file::createFileNumElements<T>(fnp, numElements);

        return boost::make_shared<memMappedFile<T> >(fnp);
    }

    static boost::shared_ptr<memMappedFile<T> >
    CreateNumBytes(const std::string& fnp, const int64_t numBytes)
    {
        om::file::resizeFileNumBytes(fnp, numBytes);

        return boost::make_shared<memMappedFile<T> >(fnp);
    }

    static boost::shared_ptr<memMappedFile<T> >
    CreateFromData(const std::string& fnp, boost::shared_ptr<T> d,
                   const int64_t numElements)
    {
        om::file::writeNumElements<T>(fnp, d, numElements);

        return boost::make_shared<memMappedFile<T> >(fnp);
    }

private:
    std::string fnp_;

    boost::shared_ptr<mapped_file> file_;

    int64_t numBytes_;

public:
    // for boost::variant
    memMappedFile()
        : fnp_(std::string(""))
    {}

    memMappedFile(const std::string& fnp)
        : fnp_(fnp)
        , numBytes_(boost::filesystem::file_size(fnp_))
    {
        file_.reset(new mapped_file(fnp_));
    }

    virtual ~memMappedFile()
    {}

    virtual uint64_t Size() const {
        return numBytes_;
    }

    virtual void Flush()
    {}

    virtual T* GetPtr() const {
        return reinterpret_cast<T*>(file_->data());
    }

    virtual T* GetPtrWithOffset(const int64_t offset) const {
        return reinterpret_cast<T*>( file_->data() + offset );
    }

    virtual std::string GetBaseFileName() const {
        return fnp_;
    }
};

} // namespace datalayer
} // namespace om
