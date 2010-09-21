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
class OmMemMappedFileReadQT : public OmIMemMappedFile<T> {
public:
	OmMemMappedFileReadQT(const std::string& fnp, const int64_t numBytes)
		: numBytes_(numBytes)
	{
		file_ = boost::make_shared<QFile>(QString::fromStdString(fnp));

		if( !file_->open(QIODevice::ReadOnly)) {
			throw OmIoException("could not open " + fnp);
		}

		checkFileSize();

		map_ = file_->map(0, file_->size());

		file_->close();

		printf("opened file %s\n", GetBaseFileName().c_str());
	}

	virtual ~OmMemMappedFileReadQT(){
		printf("closing file %s\n", qPrintable(file_->fileName()));
	}

	T* GetPtr() const {
		return reinterpret_cast< T* >( map_ );
	}

	T* GetPtrWithOffset(const int64_t offset) const {
		return reinterpret_cast< T* >( map_ + offset );
	}

	std::string GetBaseFileName() const {
		return QFileInfo(*file_).baseName().toStdString();
	}

private:
	const int64_t numBytes_;
	boost::shared_ptr<QFile> file_;
	uchar* map_;

	// optional check of expected file size
	void checkFileSize()
	{
		if(!numBytes_){
			return;
		}

		if ( file_->size() != numBytes_ ){
			const QString err =
				QString("error: input file size of %1 bytes doesn't match expected size %d")
				.arg(file_->size())
				.arg(numBytes_);
			throw OmIoException(err.toStdString());
		}
	}
};

template <typename T>
class OmMemMappedFileWriteQT : public OmIMemMappedFile<T> {
public:
	OmMemMappedFileWriteQT(const std::string& fnp,
						   const int64_t numBytes)
		: numBytes_(numBytes)
	{
		if(!numBytes){
			throw OmIoException("size was 0");
		}

		QFile::remove(QString::fromStdString(fnp));

		file_ = boost::make_shared<QFile>(QString::fromStdString(fnp));

		if(!file_->open(QIODevice::ReadWrite)){
			throw OmIoException("could not create " + fnp);
		}

		file_->resize(numBytes);

		//TODO: allocate space??

		map_ = file_->map(0, numBytes_);

		file_->close();

		printf("created file %s\n", GetBaseFileName().c_str());
	}

	virtual ~OmMemMappedFileWriteQT(){
		printf("closing file %s\n", qPrintable(file_->fileName()));
	}

	T* GetPtr() const {
		return (T*)map_;
	}

	T* GetPtrWithOffset(const int64_t offset) const {
		return (T*)(map_ + offset);
	}

	std::string GetBaseFileName() const {
		return QFileInfo(*file_).baseName().toStdString();
	}

private:
	const int64_t numBytes_;
	boost::shared_ptr<QFile> file_;
	uchar* map_;

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
