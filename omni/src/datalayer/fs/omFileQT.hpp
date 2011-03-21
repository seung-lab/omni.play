#ifndef OM_FILE_QT_HPP
#define OM_FILE_QT_HPP

#include "common/om.hpp"
#include "common/omDebug.h"
#include "datalayer/fs/omIOnDiskFile.h"
#include "utility/omSmartPtr.hpp"

#include <QFile>
#include <QFileInfo>
#include <boost/make_shared.hpp>

template <typename T>
class OmFileQTbase : public OmIOnDiskFile<T> {
protected:
    const std::string fnp_;
    boost::shared_ptr<QFile> file_;
    boost::shared_ptr<T> data_;
    zi::mutex mutex_;

    OmFileQTbase(const std::string& fnp)
        : fnp_(fnp)
    {}

    virtual ~OmFileQTbase(){
        debug(file, "closing file %s\n", GetBaseFileName().c_str());
    }

    uint64_t Size() const {
        return file_->size();
    }

    void open()
    {
        file_ = boost::make_shared<QFile>(QString::fromStdString(fnp_));

        if( !file_->open(QIODevice::ReadWrite)) {
            throw OmIoException("could not open", fnp_);
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
            throw OmIoException("could not read in fully file ", fnp_);
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
            throw OmIoException("could not write fully file", fnp_);
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
class OmFileReadQT : public OmFileQTbase<T> {
public:

    static boost::shared_ptr<OmFileReadQT<T> >
    Reader(const std::string& fnp)
    {
        OmFileReadQT* ret = new OmFileReadQT(fnp, 0);
        return boost::shared_ptr<OmFileReadQT<T> >(ret);
    }

private:
    OmFileReadQT(const std::string& fnp, const int64_t numBytes)
        : OmFileQTbase<T>(fnp)
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
            const QString err =
                QString("error: input file size of %1 bytes doesn't match expected size %d")
                .arg(this->file_->size())
                .arg(numBytes);
            throw OmIoException(err.toStdString());
        }
    }
};

template <typename T>
class OmFileWriteQT : public OmFileQTbase<T> {
public:
    static boost::shared_ptr<OmFileWriteQT<T> >
    WriterNumBytes(const std::string& fnp, const int64_t numBytes,
                   const om::ZeroMem shouldZeroFill)
    {
        OmFileWriteQT<T>* ret = new OmFileWriteQT(fnp, numBytes, shouldZeroFill);
        return boost::shared_ptr<OmFileWriteQT<T> >(ret);
    }

    static boost::shared_ptr<OmFileWriteQT<T> >
    WriterNumElements(const std::string& fnp, const int64_t numElements,
                      const om::ZeroMem shouldZeroFill)
    {
        const uint64_t numBytes = numElements*sizeof(T);
        OmFileWriteQT<T>* ret = new OmFileWriteQT(fnp, numBytes, shouldZeroFill);
        return boost::shared_ptr<OmFileWriteQT<T> >(ret);
    }

private:
    OmFileWriteQT(const std::string& fnp, const int64_t numBytes,
                  const om::ZeroMem shouldZeroFill)
        : OmFileQTbase<T>(fnp)
    {
        checkFileSize(numBytes);

        QFile::remove(QString::fromStdString(fnp));
        this->open();
        this->file_->resize(numBytes);
        //TODO: allocate space??
        const int64_t bytesRead = this->readIn();

        if(bytesRead != numBytes){
            throw OmIoException("did't read right amount of data");
        }

        if(om::ZERO_FILL == shouldZeroFill){
            memset(this->data_.get(), 0, numBytes);
        }

        debug(file, "created file %s\n", this->GetAbsFileName().c_str());
    }

    void checkFileSize(const int64_t numBytes)
    {
        if(!numBytes){
            throw OmIoException("size was 0");
        }
    }
};

#endif
