#ifndef OM_MEM_MAPPED_FILE_HPP
#define OM_MEM_MAPPED_FILE_HPP

#include <QFile>

#include <boost/make_shared.hpp>

template <typename T>
class MemMappedFileRead {
public:
	MemMappedFileRead(const std::string& fnp,
			  const qint64 numBytes)
		: fnp_(fnp), numBytes_(numBytes)
	{
		file_ = boost::make_shared<QFile>(QString::fromStdString(fnp_));

		if(!file_->open(QIODevice::ReadOnly)){
			const std::string err = "could not open " + fnp_;
			throw OmIoException(err);
		}

		if(file_->size() != (qint64)numBytes){
			const QString err =
				QString("error: input file size of %1 bytes doesn't match expected size %d")
				.arg(file_->size())
				.arg(numBytes);
			throw OmIoException(err.toStdString());
		}

		map_ = (T*)(file_->map(0, numBytes_));

		file_->close();
	}

	T* GetPtr() const {
		return map_;
	}

private:
	const std::string fnp_;
	const qint64 numBytes_;
	boost::shared_ptr<QFile> file_;
	T* map_;
};


template <typename T>
class MemMappedFileWrite {
public:
	MemMappedFileWrite(const std::string& fnp,
			   const qint64 numBytes)
		: fnp_(fnp), numBytes_(numBytes)
	{
		QFile::remove(QString::fromStdString(fnp_));

		file_ = boost::make_shared<QFile>(QString::fromStdString(fnp_));
		if(!file_->open(QIODevice::ReadWrite)){
			const std::string err = "could not open " + fnp_;
			throw OmIoException(err);
		}

		file_->resize(numBytes);

		//TODO: allocate??

		map_ = (T*)(file_->map(0, numBytes_));

		file_->close();
	}

	T* GetPtr() const {
		return map_;
	}

private:
	const std::string fnp_;
	const qint64 numBytes_;
	boost::shared_ptr<QFile> file_;
	T* map_;
};

#endif
