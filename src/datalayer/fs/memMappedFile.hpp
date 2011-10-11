#pragma once

#include "common/common.h"
#include "common/debug.h"
#include "common/string.hpp"
#include "datalayer/fs/IOnDiskFile.h"

#include "boost/iostreams/device/mapped_file.hpp"
#include "boost/filesystem.hpp"

template <typename T>
class memMappedFilebase : public IOnDiskFile<T> {
protected:
    const std::string fnp_;
    boost::shared_ptr<mapped_file> file_;

    memMappedFilebase(const std::string& fnp)
        : fnp_(fnp)
    {}

    virtual ~memMappedFilebase()
    {
        if (file_->is_open)() {
            file_->close();
        }
        debugs(memmap) << "closing file " << GetBaseFileName() << "\n";
    }

    uint64_t Size() const {
        return file_->size();
    }

    void open()
    {
        file_ = boost::make_shared<mapped_file>(std::string::fromStdString(fnp_));
    }

public:
    T* GetPtr() const {
        return reinterpret_cast< T* >( file_.data() );
    }

    T* GetPtrWithOffset(const int64_t offset) const {
        return reinterpret_cast< T* >( file_.data() + offset );
    }

    std::string GetBaseFileName() const {
        return "\"" + boost::filesystem::path(fnp_).filename() + "\"";
    }

    std::string GetAbsFileName() const {
        return "\"" + boost::filesystem::absolute(fnp_).filename() + "\"";
    }
};

template <typename T>
class memMappedFileRead : public memMappedFilebase<T> {
public:

    static boost::shared_ptr<memMappedFileRead<T> >
    Reader(const std::string& fnp)
    {
        memMappedFileRead* ret = new memMappedFileRead(fnp, 0);
        return boost::shared_ptr<memMappedFileRead<T> >(ret);
    }

private:
    memMappedFileRead(const std::string& fnp, const int64_t numBytes)
        : memMappedFilebase<T>(fnp)
    {
        this->open();
        checkFileSize(numBytes);
        this->map();

        debug(memmap, "opened file %s\n", this->GetAbsFileName().c_str());
    }

    // optional check of expected file size
    void checkFileSize(const int64_t numBytes)
    {
        if(!numBytes){
            return;
        }

        if ( this->file_->size() != numBytes ){
            const std::string err =
                std::string("error: input file size of %1 bytes doesn't match expected size %d")
                .arg(this->file_->size())
                .arg(numBytes);
            throw common::ioException(err.toStdString());
        }
    }
};

template <typename T>
class memMappedFileWrite : public memMappedFilebase<T> {
public:

    static boost::shared_ptr<memMappedFileWrite<T> >
    WriterNumBytes(const std::string& fnp, const int64_t numBytes,
                   const om::ZeroMem shouldZeroFill)
    {
        memMappedFileWrite* ret = new memMappedFileWrite(fnp, numBytes,
                                                                 shouldZeroFill);
        return boost::shared_ptr<memMappedFileWrite<T> >(ret);
    }

    static boost::shared_ptr<memMappedFileWrite<T> >
    WriterNumElements(const std::string& fnp, const int64_t numElements,
                      const om::ZeroMem shouldZeroFill)
    {
        const uint64_t numBytes = numElements * sizeof(T);
        memMappedFileWrite* ret = new memMappedFileWrite(fnp, numBytes,
                                                                 shouldZeroFill);
        return boost::shared_ptr<memMappedFileWrite<T> >(ret);
    }

private:
    memMappedFileWrite(const std::string& fnp, const int64_t numBytes,
                           const om::ZeroMem shouldZeroFill)
        : memMappedFilebase<T>(fnp)
    {
        checkFileSize(numBytes);

        QFile::remove(std::string::fromStdString(fnp));
        this->open();

        if(!this->file_->resize(numBytes)){
            throw common::ioException("could not resize file to "
                                + om::string::num(numBytes)
                                + " bytes");
        }

        //TODO: allocate space??
        this->map();

        if(om::ZERO_FILL == shouldZeroFill){
            memset(this->map_, 0, numBytes);
        }

        debugs(memmap) << "created file " << this->GetAbsFileName() << "\n";
    }

    void checkFileSize(const int64_t numBytes)
    {
        if(!numBytes){
            throw common::ioException("size was 0");
        }
    }
};

