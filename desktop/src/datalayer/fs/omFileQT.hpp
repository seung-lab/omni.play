#pragma once

#include "common/logging.h"
#include "datalayer/fs/omIOnDiskFile.h"
#include "utility/malloc.hpp"

#include <QFile>
#include <QFileInfo>
#include <boost/make_shared.hpp>

template <typename T> class OmFileQTbase : public OmIOnDiskFile<T> {
 protected:
  const std::string fnp_;
  std::shared_ptr<QFile> file_;
  std::shared_ptr<T> data_;
  zi::mutex mutex_;

  OmFileQTbase(const std::string& fnp) : fnp_(fnp) {}

  virtual ~OmFileQTbase() {}

  uint64_t Size() const { return file_->size(); }

  void open() {
    file_ = std::make_shared<QFile>(QString::fromStdString(fnp_));

    if (!file_->open(QIODevice::ReadWrite)) {
      throw om::IoException("could not open", fnp_);
    }
  }

  uint64_t readIn() {
    const uint64_t numBytes = file_->size();

    data_ = om::mem::Malloc<T>::NumBytes(numBytes, om::mem::ZeroFill::DONT);
    char* dataCharPtr = reinterpret_cast<char*>(data_.get());
    file_->seek(0);
    const uint64_t readBytes = file_->read(dataCharPtr, numBytes);
    if (readBytes != numBytes) {
      throw om::IoException("could not read in fully file ", fnp_);
    }

    return readBytes;
  }

 public:
  virtual void Flush() {
    zi::guard g(mutex_);

    const uint64_t numBytes = file_->size();

    char* dataCharPtr = reinterpret_cast<char*>(data_.get());
    file_->seek(0);
    const uint64_t writeBytes = file_->write(dataCharPtr, numBytes);
    if (writeBytes != numBytes) {
      throw om::IoException("could not write fully file", fnp_);
    }
    printf("flushed %s\n", fnp_.c_str());
  }

  T* GetPtr() const { return data_.get(); }

  T* GetPtrWithOffset(const int64_t offset) const {
    uchar* dataCharPtr = reinterpret_cast<uchar*>(data_.get());
    return reinterpret_cast<T*>(dataCharPtr + offset);
  }

  uint64_t FileSizeBytes() const { return file_->size(); }

  std::string GetBaseFileName() const {
    return "\"" + QFileInfo(*file_).fileName().toStdString() + "\"";
  }

  std::string GetAbsFileName() const {
    return "\"" + QFileInfo(*file_).absoluteFilePath().toStdString() + "\"";
  }
};

template <typename T> class OmFileReadQT : public OmFileQTbase<T> {
 public:

  static std::shared_ptr<OmFileReadQT<T> > Reader(const std::string& fnp) {
    OmFileReadQT* ret = new OmFileReadQT(fnp, 0);
    return std::shared_ptr<OmFileReadQT<T> >(ret);
  }

 private:
  OmFileReadQT(const std::string& fnp, const int64_t numBytes)
      : OmFileQTbase<T>(fnp) {
    this->open();
    checkFileSize(numBytes);
    this->readIn();

    //debug(file, "opened file %s\n", this->GetAbsFileName().c_str());
  }

  // optional check of expected file size
  void checkFileSize(const int64_t numBytes) {
    if (!numBytes) {
      return;
    }

    if (this->file_->size() != numBytes) {
      const QString err = QString(
          "error: input file size of %1 bytes doesn't match expected size %d")
          .arg(this->file_->size()).arg(numBytes);
      throw om::IoException(err.toStdString());
    }
  }
};

template <typename T> class OmFileWriteQT : public OmFileQTbase<T> {
 public:
  static std::shared_ptr<OmFileWriteQT<T> > WriterNumBytes(
      const std::string& fnp, const int64_t numBytes,
      const om::mem::ZeroFill shouldZeroFill) {
    OmFileWriteQT<T>* ret = new OmFileWriteQT(fnp, numBytes, shouldZeroFill);
    return std::shared_ptr<OmFileWriteQT<T> >(ret);
  }

  static std::shared_ptr<OmFileWriteQT<T> > WriterNumElements(
      const std::string& fnp, const int64_t numElements,
      const om::mem::ZeroFill shouldZeroFill) {
    const uint64_t numBytes = numElements * sizeof(T);
    OmFileWriteQT<T>* ret = new OmFileWriteQT(fnp, numBytes, shouldZeroFill);
    return std::shared_ptr<OmFileWriteQT<T> >(ret);
  }

 private:
  OmFileWriteQT(const std::string& fnp, const int64_t numBytes,
                const om::mem::ZeroFill shouldZeroFill)
      : OmFileQTbase<T>(fnp) {
    checkFileSize(numBytes);

    QFile::remove(QString::fromStdString(fnp));
    this->open();
    this->file_->resize(numBytes);
    //TODO: allocate space??
    const int64_t bytesRead = this->readIn();

    if (bytesRead != numBytes) {
      throw om::IoException("did't read right amount of data");
    }

    if (om::mem::ZeroFill::ZERO == shouldZeroFill) {
      memset(this->data_.get(), 0, numBytes);
    }

    //debug(file, "created file %s\n", this->GetAbsFileName().c_str());
  }

  void checkFileSize(const int64_t numBytes) {
    if (!numBytes) {
      throw om::IoException("size was 0");
    }
  }
};
