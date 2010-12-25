#ifndef OM_MESH_CHUNK_DATA_READER_V2_HPP
#define OM_MESH_CHUNK_DATA_READER_V2_HPP

#include "common/omCommon.h"
#include "datalayer/fs/omFileNames.hpp"
#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include "mesh/io/v2/chunk/omMeshChunkTypes.h"

class OmMeshChunkDataReaderV2{
private:
	OmSegmentation* seg_;
	const OmMipChunkCoord& coord_;
	const double threshold_;
	const QString fnp_;

public:
	OmMeshChunkDataReaderV2(OmSegmentation* seg, const OmMipChunkCoord& coord)
		: seg_(seg)
		, coord_(coord)
		, threshold_(1.)
		, fnp_(filePath(threshold_))
	{}

	~OmMeshChunkDataReaderV2()
	{}

	// no locking needed
	template <typename T>
	boost::shared_ptr<T> Read(const OmMeshFilePart& entry)
	{
		const int64_t numBytes = entry.totalBytes;

		assert(numBytes);

		boost::shared_ptr<T> ret =
			OmSmartPtr<T>::MallocNumBytes(numBytes, om::DONT_ZERO_FILL);

		char* dataCharPtr = reinterpret_cast<char*>(ret.get());

		QFile reader(fnp_);
		if( !reader.open(QIODevice::ReadOnly)) {
			throw OmIoException("could not open", fnp_);
		}

		if(!reader.seek(entry.offsetIntoFile)){
			throw OmIoException("could not seek to " +
								om::NumToStr(entry.offsetIntoFile));
		}

		const int64_t bytesRead = reader.read(dataCharPtr, numBytes);
		if(bytesRead != numBytes){
			std::cout << "could not read data; numBytes is " << numBytes
					  << ", but only read " << bytesRead << "\n"
					  << std::flush;
			throw OmIoException("could not read fully file", fnp_);
		}

		return ret;
	}

private:
	QString filePath(const double threshold)
	{
		const QString volPath =
			OmFileNames::GetMeshChunkFolderPath(seg_, threshold, coord_);
		const QString fullPath = QString("%1meshData.ver2")
			.arg(volPath);
		return fullPath;
	}
};

#endif
