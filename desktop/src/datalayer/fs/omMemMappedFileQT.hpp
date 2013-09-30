#pragma once

#include "common/common.h"
#include "common/enums.hpp"
#include "common/logging.h"
#include "common/string.hpp"
#include "datalayer/fs/omIOnDiskFile.h"

#include <QFile>
#include <QFileInfo>

template <typename T>
class OmMemMappedFileQTbase : public OmIOnDiskFile<T> {
protected:
    const std::string fnp_;
    std::shared_ptr<QFile> file_;
    uchar* map_;

    OmMemMappedFileQTbase(const std::string& fnp)
        : fnp_(fnp)
    {}

    virtual ~OmMemMappedFileQTbase()
    {
        file_->unmap(map_);
        //debugs(memmap) << "closing file " << GetBaseFileName() << "\n";
    }

    uint64_t Size() const {
        return file_->size();
    }

    void open()
    {
        file_ = std::make_shared<QFile>(QString::fromStdString(fnp_));

        if(!file_->open(QIODevice::ReadWrite)) {
            throw om::IoException("could not open", fnp_);
        }
    }

    void map()
    {
        //std::cout << "file size is " << file_->size() << "\n";
        map_ = file_->map(0, file_->size());
        if(!map_){
            throw om::IoException("could not map file");
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
class OmMemMappedFileReadQT : public OmMemMappedFileQTbase<T> {
public:

    static std::shared_ptr<OmMemMappedFileReadQT<T> >
    Reader(const std::string& fnp)
    {
        OmMemMappedFileReadQT* ret = new OmMemMappedFileReadQT(fnp, 0);
        return std::shared_ptr<OmMemMappedFileReadQT<T> >(ret);
    }

private:
    OmMemMappedFileReadQT(const std::string& fnp, const int64_t numBytes)
        : OmMemMappedFileQTbase<T>(fnp)
    {
        this->open();
        checkFileSize(numBytes);
        this->map();

        //debug(memmap, "opened file %s\n", this->GetAbsFileName().c_str());
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
            throw om::IoException(err.toStdString());
        }
    }
};

template <typename T>
class OmMemMappedFileWriteQT : public OmMemMappedFileQTbase<T> {
public:

    static std::shared_ptr<OmMemMappedFileWriteQT<T> >
    WriterNumBytes(const std::string& fnp, const int64_t numBytes,
                   const om::common::ZeroMem shouldZeroFill)
    {
        OmMemMappedFileWriteQT* ret = new OmMemMappedFileWriteQT(fnp, numBytes,
                                                                 shouldZeroFill);
        return std::shared_ptr<OmMemMappedFileWriteQT<T> >(ret);
    }

    static std::shared_ptr<OmMemMappedFileWriteQT<T> >
    WriterNumElements(const std::string& fnp, const int64_t numElements,
                      const om::common::ZeroMem shouldZeroFill)
    {
        const uint64_t numBytes = numElements * sizeof(T);
        OmMemMappedFileWriteQT* ret = new OmMemMappedFileWriteQT(fnp, numBytes,
                                                                 shouldZeroFill);
        return std::shared_ptr<OmMemMappedFileWriteQT<T> >(ret);
    }

private:
    OmMemMappedFileWriteQT(const std::string& fnp, const int64_t numBytes,
                           const om::common::ZeroMem shouldZeroFill)
        : OmMemMappedFileQTbase<T>(fnp)
    {
        checkFileSize(numBytes);

        QFile::remove(QString::fromStdString(fnp));
        this->open();

        if(!this->file_->resize(numBytes)){
            throw om::IoException("could not resize file to "
                                + om::string::num(numBytes)
                                + " bytes");
        }

        //TODO: allocate space??
        this->map();

        if(om::common::ZeroMem::ZERO_FILL == shouldZeroFill){
            memset(this->map_, 0, numBytes);
        }

        //debugs(memmap) << "created file " << this->GetAbsFileName() << "\n";
    }

    void checkFileSize(const int64_t numBytes)
    {
        if(!numBytes){
            throw om::IoException("size was 0");
        }
    }
};

