#pragma once

#include "common/om.hpp"
#include "common/debug.h"
#include "datalayer/fs/IOnDiskFile.h"
#include "utility/omSmartPtr.hpp"

#include <QFile>
#include <QFileInfo>
#include <boost/make_shared.hpp>

template <typename T>
class fileQTbase : public IOnDiskFile<T> {
protected:
    const std::string fnp_;
    boost::shared_ptr<QFile> file_;
    boost::shared_ptr<T> data_;
    zi::mutex mutex_;

    fileQTbase(const std::string& fnp)
        : fnp_(fnp)
    {}

    virtual ~fileQTbase()
    {}

    uint64_t Size() const {
        return file_->size();
    }

    void open()
    {
        file_ = boost::make_shared<QFile>(std::string::fromStdString(fnp_));

        if( !file_->open(QIODevice::ReadWrite)) {
            throw common::ioException("could not open", fnp_);
        }
    }

    uint64_t readIn()
    {
        const uint64_t numBytes = file_->size();

        data_ = OmSmartPtr<T>::MallocNumBytes(numBytes, om::DONT_ZERO_FILL);
        char* dataCharPtr = reinterpret_cast<char*>(data_.get());
        file_->seek(0);
        const uint64_t readBytes = file_->read(dataCharPtr, numBytes);
        if(readBytes != numBytes){
            throw common::ioException("could not read in fully file ", fnp_);
        }

        return readBytes;
    }

public:
    virtual void Flush()
    {
        zi::guard g(mutex_);

        const uint64_t numBytes = file_->size();

        char* dataCharPtr = reinterpret_cast<char*>(data_.get());
        file_->seek(0);
        const uint64_t writeBytes = file_->write(dataCharPtr, numBytes);
        if(writeBytes != numBytes){
            throw common::ioException("could not write fully file", fnp_);
        }
        printf("flushed %s\n", fnp_.c_str());
    }

    T* GetPtr() const {
        return data_.get();
    }

    T* GetPtrWithOffset(const int64_t offset) const {
        uchar* dataCharPtr = reinterpret_cast<uchar*>(data_.get());
        return reinterpret_cast<T*>(dataCharPtr + offset);
    }

    uint64_t FileSizeBytes() const {
        return file_->size();
    }

    std::string GetBaseFileName() const {
        return "\"" + QFileInfo(*file_).fileName().toStdString() + "\"";
    }

    std::string GetAbsFileName() const {
        return "\"" + QFileInfo(*file_).absoluteFilePath().toStdString() + "\"";
    }
};

template <typename T>
class fileReadQT : public fileQTbase<T> {
public:

    static boost::shared_ptr<fileReadQT<T> >
    Reader(const std::string& fnp)
    {
        fileReadQT* ret = new fileReadQT(fnp, 0);
        return boost::shared_ptr<fileReadQT<T> >(ret);
    }

private:
    fileReadQT(const std::string& fnp, const int64_t numBytes)
        : fileQTbase<T>(fnp)
    {
        this->open();
        checkFileSize(numBytes);
        this->readIn();

        debug(file, "opened file %s\n", this->GetAbsFileName().c_str());
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
class fileWriteQT : public fileQTbase<T> {
public:
    static boost::shared_ptr<fileWriteQT<T> >
    WriterNumBytes(const std::string& fnp, const int64_t numBytes,
                   const om::ZeroMem shouldZeroFill)
    {
        fileWriteQT<T>* ret = new fileWriteQT(fnp, numBytes, shouldZeroFill);
        return boost::shared_ptr<fileWriteQT<T> >(ret);
    }

    static boost::shared_ptr<fileWriteQT<T> >
    WriterNumElements(const std::string& fnp, const int64_t numElements,
                      const om::ZeroMem shouldZeroFill)
    {
        const uint64_t numBytes = numElements*sizeof(T);
        fileWriteQT<T>* ret = new fileWriteQT(fnp, numBytes, shouldZeroFill);
        return boost::shared_ptr<fileWriteQT<T> >(ret);
    }

private:
    fileWriteQT(const std::string& fnp, const int64_t numBytes,
                  const om::ZeroMem shouldZeroFill)
        : fileQTbase<T>(fnp)
    {
        checkFileSize(numBytes);

        QFile::remove(std::string::fromStdString(fnp));
        this->open();
        this->file_->resize(numBytes);
        //TODO: allocate space??
        const int64_t bytesRead = this->readIn();

        if(bytesRead != numBytes){
            throw common::ioException("did't read right amount of data");
        }

        if(om::ZERO_FILL == shouldZeroFill){
            memset(this->data_.get(), 0, numBytes);
        }

        debug(file, "created file %s\n", this->GetAbsFileName().c_str());
    }

    void checkFileSize(const int64_t numBytes)
    {
        if(!numBytes){
            throw common::ioException("size was 0");
        }
    }
};

