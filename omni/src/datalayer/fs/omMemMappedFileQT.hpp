#ifndef OM_MEM_MAPPED_FILE_QT_HPP
#define OM_MEM_MAPPED_FILE_QT_HPP

#include <QFile>
#include <QFileInfo>
#include <boost/make_shared.hpp>

template <typename T>
class OmIMemMappedFile {
public:
	virtual ~OmIMemMappedFile(){}

	virtual T* GetPtr() const = 0;
	virtual T* GetPtrWithOffset(const int64_t offset) const = 0;
	virtual std::string GetBaseFileName() const = 0;
};

template <typename T>
class OmMemMappedFileQTbase : public OmIMemMappedFile<T> {
protected:
	const std::string fnp_;
	boost::shared_ptr<QFile> file_;
	uchar* map_;

	OmMemMappedFileQTbase(const std::string& fnp)
		: fnp_(fnp)
	{}

	virtual ~OmMemMappedFileQTbase(){
		debug(memmap, "closing file %s\n", GetBaseFileName().c_str());
	}

	void open(const QIODevice::OpenModeFlag mode)
	{
		file_ = boost::make_shared<QFile>(QString::fromStdString(fnp_));

		if( !file_->open(mode)) {
			throw OmIoException("could not open " + fnp_);
		}
	}

	void map()
	{
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
};

template <typename T>
class OmMemMappedFileReadQT : public OmMemMappedFileQTbase<T> {
public:
	OmMemMappedFileReadQT(const std::string& fnp, const int64_t numBytes)
		: OmMemMappedFileQTbase<T>(fnp)
	{
		this->open(QIODevice::ReadWrite);
		checkFileSize(numBytes);
		this->map();

		debug(memmap, "opened file %s\n", this->GetBaseFileName().c_str());
	}

private:
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
	OmMemMappedFileWriteQT(const std::string& fnp, const int64_t numBytes)
		: OmMemMappedFileQTbase<T>(fnp)
	{
		checkFileSize(numBytes);

		QFile::remove(QString::fromStdString(fnp));
		this->open(QIODevice::QIODevice::ReadWrite);
		this->file_->resize(numBytes);
		//TODO: allocate space??
		this->map();

		//debug(io, "created file %s\n", this->GetBaseFileName().c_str());
	}

private:
	void checkFileSize(const int64_t numBytes)
	{
		if(!numBytes){
			throw OmIoException("size was 0");
		}
	}
	/*

	  template <typename T, typename VOL>
	  void OmMemMappedVolume<T,VOL>::allocateSpace(QFile * file)
	  {
	  printf("\tpre-allocating...\n");
	  for( int64_t i=0; i < file->size(); i+=(int64_t)4096){
	  file->seek(i);
	  file->putChar(0);
	  }
	  printf("\tflushing...\n");
	  file->flush();
	  }
	*/
};

#endif
