#pragma once

#include "datalayer/fs/omFileQT.hpp"

template <typename T>
class OmVectorInFile {
private:
	const QString fnp_;
	std::vector<T> vec_;

public:
	OmVectorInFile(const QString& fnp)
		: fnp_(fnp)
	{}

	std::vector<T>& Vector(){
		return vec_;
	}

	bool Exists() const	{
		return QFile::exists(fnp_);
	}

	void Load()
	{
		std::shared_ptr<OmFileReadQT<T> > reader =
			OmFileReadQT<T>::Reader(fnp_.toStdString());

		const uint64_t numBytes = reader->FileSizeBytes();
		const uint32_t numElements = numBytes / sizeof(T);
		vec_.resize(numElements);
		memcpy(&vec_[0], reader->GetPtr(), numBytes);
	}

	void Save()
	{
		const uint32_t numElements = vec_.size();
		const uint64_t numBytes = numElements * sizeof(T);

		std::shared_ptr<OmFileWriteQT<T> > writer =
			OmFileWriteQT<T>::WriterNumBytes(fnp_.toStdString(),
											 numBytes,
											 om::mem::ZeroFill::DONT);
		memcpy(writer->GetPtr(), &vec_[0], numBytes);
		writer->Flush();
	}
};

