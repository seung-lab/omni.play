#ifndef OM_MEM_MAPPED_FILE_QT_HPP
#define OM_MEM_MAPPED_FILE_QT_HPP

#include <QFile>
#include <boost/make_shared.hpp>

#include <cstddef>

template <typename T>
class OmIMemMappedFile {
public:
	virtual ~OmIMemMappedFile(){}

	virtual T* GetPtr() const = 0;
	virtual T* GetPtrWithOffset(const int64_t offset) const = 0;
};

template <typename T>
class OmMemMappedFileReadQT : public OmIMemMappedFile<T> {
public:
	OmMemMappedFileReadQT(const std::string& fnp, const int64_t numBytes)
		: fnp_(fnp)
	{
		file_ = boost::make_shared<QFile>(QString::fromStdString(fnp_));

		if( !file_->open(QIODevice::ReadOnly))
		{
			const std::string err = "could not open " + fnp_;
			throw OmIoException(err);
		}

		if(numBytes){ // perform optional check of expected file size
			checkFileSize(numBytes);
		}

		map_ = file_->map(0, file_->size());

		file_->close();
	}

	virtual ~OmMemMappedFileReadQT(){
		printf("closing file %s\n", fnp_.c_str());
	}

	T* GetPtr() const {
		return reinterpret_cast< T* >( map_ );
	}

	T* GetPtrWithOffset(const int64_t offset) const {
		return reinterpret_cast< T* >( map_ + offset );
	}

private:
	const std::string fnp_;
	boost::shared_ptr<QFile> file_;
	uchar* map_;

	void checkFileSize(const int64_t numBytes)
	{
		if ( file_->size() != numBytes ){
			const QString err =
				QString("error: input file size of %1 bytes doesn't match expected size %d")
				.arg(file_->size())
				.arg(numBytes);
			throw OmIoException(err.toStdString());
		}
	}
};

template <typename T>
class OmMemMappedFileWriteQT : public OmIMemMappedFile<T> {
public:
	OmMemMappedFileWriteQT(const std::string& fnp,
						   const int64_t numBytes)
		: fnp_(fnp), numBytes_(numBytes)
	{
		if(!numBytes){
			throw OmIoException("size was 0");
		}

		QFile::remove(QString::fromStdString(fnp_));

		file_ = boost::make_shared<QFile>(QString::fromStdString(fnp_));

		if(!file_->open(QIODevice::ReadWrite)){
			const std::string err = "could not open " + fnp_;
			throw OmIoException(err);
		}

		file_->resize(numBytes);

		//TODO: allocate space??

		map_ = file_->map(0, numBytes_);

		file_->close();
	}

	virtual ~OmMemMappedFileWriteQT(){
		printf("closing file %s\n", fnp_.c_str());
	}

	T* GetPtr() const {
		return (T*)map_;
	}

	T* GetPtrWithOffset(const int64_t offset) const {
		return (T*)(map_ + offset);
	}

private:
	const std::string fnp_;
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
