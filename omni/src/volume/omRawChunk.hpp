#ifndef OM_RAW_CHUNK_HPP
#define OM_RAW_CHUNK_HPP

#include "zi/omMutex.h"
#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"
#include "volume/omMipChunkCoord.h"
#include "datalayer/fs/omFileNames.hpp"
#include "utility/omSmartPtr.hpp"

#include <boost/make_shared.hpp>
#include <QFile>

template <typename T>
class OmRawChunk {
private:
	OmMipVolume *const vol_;
	const OmMipChunkCoord coord_;
	const uint64_t chunkOffset_;
	const QString memMapFileName_;
	const uint64_t numBytes_;

	boost::shared_ptr<T> data_;
	T* dataRaw_;

	bool dirty_;

public:
	OmRawChunk(OmMipVolume* vol, const OmMipChunkCoord& coord)
		: vol_(vol)
		, coord_(coord)
		, chunkOffset_(vol_->ComputeChunkPtrOffset(coord_))
		, memMapFileName_(OmFileNames::GetMemMapFileNameQT(vol_,
														   coord.Level))
		, numBytes_(128*128*128*vol_->GetBytesPerSample())
		, dataRaw_(NULL)
		, dirty_(false)
	{
		readData();
	}

	~OmRawChunk()
	{
		if(dirty_){
			std::cout << "flushing " << coord_ << "\n";
			writeData();
		}
	}

	inline void Set(const uint64_t index, const T val)
	{
		dirty_ = true;
		dataRaw_[index] = val;
	}

	inline T Get(const uint64_t index) const
	{
		return dataRaw_[index];
	}

private:
	void readData()
	{
		QFile file(memMapFileName_);
		if(!file.open(QIODevice::ReadOnly)){
			throw OmIoException("could not open", memMapFileName_);
		}

		file.seek(chunkOffset_);

		data_ = OmSmartPtr<T>::MallocNumBytes(numBytes_, om::DONT_ZERO_FILL);
		char* dataAsCharPtr = (char*)(data_.get());
		const uint64_t readSize = file.read(dataAsCharPtr, numBytes_);

		if( readSize != numBytes_) {
			throw OmIoException("read failed");
		}

		dataRaw_ = data_.get();
	}

	void writeData()
	{
		QFile file(memMapFileName_);
		if(!file.open(QIODevice::ReadWrite)){
			throw OmIoException("could not open", memMapFileName_);
		}

		file.seek(chunkOffset_);
		const uint64_t writeSize =
			file.write(reinterpret_cast<const char*>(dataRaw_), numBytes_);

		if( writeSize != numBytes_) {
			throw OmIoException("write failed");
		}

		dirty_ = false;
	}
};

#endif
