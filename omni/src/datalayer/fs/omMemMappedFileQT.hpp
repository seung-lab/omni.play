#ifndef OM_MEM_MAPPED_FILE_QT_HPP
#define OM_MEM_MAPPED_FILE_QT_HPP

#include "common/om.hpp"
#include "common/omDebug.h"
#include "datalayer/fs/omIOnDiskFile.h"

#include <QFile>
#include <QFileInfo>
#include <boost/make_shared.hpp>

template <typename T>
class OmMemMappedFileQTbase : public OmIOnDiskFile<T> {
protected:
	const std::string fnp_;
	boost::shared_ptr<QFile> file_;
	uchar* map_;

	OmMemMappedFileQTbase(const std::string& fnp)
		: fnp_(fnp)
	{}

	virtual ~OmMemMappedFileQTbase(){
		file_->unmap(map_);
		debug(memmap, "closing file %s\n", GetBaseFileName().c_str());
	}

	uint64_t Size(){
		boost::shared_ptr<QFile> file =
			boost::make_shared<QFile>(QString::fromStdString(fnp_));
		return file->size();
	}

	void open()
	{
		file_ = boost::make_shared<QFile>(QString::fromStdString(fnp_));

		if( !file_->open(QIODevice::ReadWrite)) {
			throw OmIoException("could not open " + fnp_);
		}
	}

	void map()
	{
		//std::cout << "file size is " << file_->size() << "\n";
		map_ = file_->map(0, file_->size());
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

	static boost::shared_ptr<OmMemMappedFileReadQT<T> >
	Reader(const std::string& fnp)
	{
		OmMemMappedFileReadQT* ret = new OmMemMappedFileReadQT(fnp, 0);
		return boost::shared_ptr<OmMemMappedFileReadQT<T> >(ret);
	}

private:
	OmMemMappedFileReadQT(const std::string& fnp, const int64_t numBytes)
		: OmMemMappedFileQTbase<T>(fnp)
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
			const QString err =
				QString("error: input file size of %1 bytes doesn't match expected size %d")
				.arg(this->file_->size())
				.arg(numBytes);
			throw OmIoException(err.toStdString());
		}
	}
};

template <typename T>
class OmMemMappedFileWriteQT : public OmMemMappedFileQTbase<T> {
public:

	static boost::shared_ptr<OmMemMappedFileWriteQT<T> >
	WriterNumBytes(const std::string& fnp, const int64_t numBytes,
						   const om::zeroMem shouldZeroFill)
	{
		OmMemMappedFileWriteQT* ret = new OmMemMappedFileWriteQT(fnp, numBytes,
																 shouldZeroFill);
		return boost::shared_ptr<OmMemMappedFileWriteQT<T> >(ret);
	}

	static boost::shared_ptr<OmMemMappedFileWriteQT<T> >
	WriterNumElements(const std::string& fnp, const int64_t numElements,
					  const om::zeroMem shouldZeroFill)
	{
		const uint64_t numBytes = numElements * sizeof(T);
		OmMemMappedFileWriteQT* ret = new OmMemMappedFileWriteQT(fnp, numBytes,
																 shouldZeroFill);
		return boost::shared_ptr<OmMemMappedFileWriteQT<T> >(ret);
	}

private:
	OmMemMappedFileWriteQT(const std::string& fnp, const int64_t numBytes,
						   const om::zeroMem shouldZeroFill)
		: OmMemMappedFileQTbase<T>(fnp)
	{
		checkFileSize(numBytes);

		QFile::remove(QString::fromStdString(fnp));
		this->open();
		this->file_->resize(numBytes);
		//TODO: allocate space??
		this->map();

		if(om::ZERO_FILL == shouldZeroFill){
			memset(this->map_, 0, numBytes);
		}

		debug(memmap, "created file %s\n", this->GetAbsFileName().c_str());
	}

	void checkFileSize(const int64_t numBytes)
	{
		if(!numBytes){
			throw OmIoException("size was 0");
		}
	}
};

#endif
