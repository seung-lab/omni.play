#pragma once

#include "common/om.hpp"
#include "common/common.h"
#include "common/debug.h"
#include "common/omString.hpp"
#include "datalayer/fs/IOnDiskFile.h"

#include <QFile>
#include <QFileInfo>

template <typename T>
class memMappedFileQTbase : public IOnDiskFile<T> {
protected:
    const std::string fnp_;
    boost::shared_ptr<QFile> file_;
    uchar* map_;

    memMappedFileQTbase(const std::string& fnp)
        : fnp_(fnp)
    {}

    virtual ~memMappedFileQTbase()
    {
        file_->unmap(map_);
        debugs(memmap) << "closing file " << GetBaseFileName() << "\n";
    }

    uint64_t Size() const {
        return file_->size();
    }

    void open()
    {
        file_ = om::make_shared<QFile>(std::string::fromStdString(fnp_));

        if(!file_->open(QIODevice::ReadWrite)) {
            throw OmIoException("could not open", fnp_);
        }
    }

    void map()
    {
        //std::cout << "file size is " << file_->size() << "\n";
        map_ = file_->map(0, file_->size());
        if(!map_){
            throw OmIoException("could not map file", file_->fileName());
        }
        file_->close();
    }

public:
    T* GetPtr() const {
        return reinterpret_cast< T* >( map_ );
    }

    T* GetPtrWithOffset(const int64_t offset) const {
        return reinterpret_cast< T* >( map_ + offset );
    }

    std::string GetBaseFileName() const {
        return "\"" + QFileInfo(*file_).fileName().toStdString() + "\"";
    }

    std::string GetAbsFileName() const {
        return "\"" + QFileInfo(*file_).absoluteFilePath().toStdString() + "\"";
    }
};

template <typename T>
class OmMemMappedFileReadQT : public memMappedFileQTbase<T> {
public:

    static boost::shared_ptr<OmMemMappedFileReadQT<T> >
    Reader(const std::string& fnp)
    {
        OmMemMappedFileReadQT* ret = new OmMemMappedFileReadQT(fnp, 0);
        return boost::shared_ptr<OmMemMappedFileReadQT<T> >(ret);
    }

private:
    OmMemMappedFileReadQT(const std::string& fnp, const int64_t numBytes)
        : memMappedFileQTbase<T>(fnp)
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
            throw OmIoException(err.toStdString());
        }
    }
};

template <typename T>
class OmMemMappedFileWriteQT : public memMappedFileQTbase<T> {
public:

    static boost::shared_ptr<OmMemMappedFileWriteQT<T> >
    WriterNumBytes(const std::string& fnp, const int64_t numBytes,
                   const om::ZeroMem shouldZeroFill)
    {
        OmMemMappedFileWriteQT* ret = new OmMemMappedFileWriteQT(fnp, numBytes,
                                                                 shouldZeroFill);
        return boost::shared_ptr<OmMemMappedFileWriteQT<T> >(ret);
    }

    static boost::shared_ptr<OmMemMappedFileWriteQT<T> >
    WriterNumElements(const std::string& fnp, const int64_t numElements,
                      const om::ZeroMem shouldZeroFill)
    {
        const uint64_t numBytes = numElements * sizeof(T);
        OmMemMappedFileWriteQT* ret = new OmMemMappedFileWriteQT(fnp, numBytes,
                                                                 shouldZeroFill);
        return boost::shared_ptr<OmMemMappedFileWriteQT<T> >(ret);
    }

private:
    OmMemMappedFileWriteQT(const std::string& fnp, const int64_t numBytes,
                           const om::ZeroMem shouldZeroFill)
        : memMappedFileQTbase<T>(fnp)
    {
        checkFileSize(numBytes);

        QFile::remove(std::string::fromStdString(fnp));
        this->open();

        if(!this->file_->resize(numBytes)){
            throw OmIoException("could not resize file to "
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
            throw OmIoException("size was 0");
        }
    }
};

