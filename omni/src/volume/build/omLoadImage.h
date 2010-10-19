#ifndef OM_LOAD_IMAGE_H
#define OM_LOAD_IMAGE_H

#include "common/omCommon.h"
#include "utility/stringHelpers.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omMipVolume.h"
#include "utility/omTimer.h"

#include <QImage>

template <typename VOL>
class OmLoadImage {
private:
	VOL *const vol_;
	const boost::shared_ptr<QFile> mip0volFile_;
	const Vector3i mip0dims_;
	const int totalNumImages_;
	const int totalTilesInSlice_;
	const uint64_t sliceWidth_;
	const uint64_t sliceHeight_;
	const uint64_t slicesPerChunk_;
	const uint64_t sliceSizeBytes_;
	const uint64_t chunkSizeBytes_;
	QString mMsg;

public:
	OmLoadImage(VOL* vol, boost::shared_ptr<QFile> mip0volFile)
		: vol_(vol)
		, mip0volFile_(mip0volFile)
		, mip0dims_(vol_->MipLevelDimensionsInMipChunks(0))
		, totalNumImages_(vol_->mSourceFilenamesAndPaths.size())
		, totalTilesInSlice_(mip0dims_.x * mip0dims_.y)
		, sliceWidth_(vol_->GetChunkDimension())
		, sliceHeight_(vol_->GetChunkDimension())
		, slicesPerChunk_(vol_->GetChunkDimension())
		, sliceSizeBytes_(sliceWidth_*sliceHeight_*vol_->GetBytesPerSample())
		, chunkSizeBytes_(sliceSizeBytes_*slicesPerChunk_)
	{}

	void processSlice(const QString & fn, const int sliceNum )
	{
		OmTimer timer;

		mMsg = QString("\r(%1 of %2) loading image %3...").arg(sliceNum+1).arg(totalNumImages_).arg(fn);
		printf("\n%s", qPrintable(fn));
		fflush(stdout);

		QFile file(fn);
		if(!file.open(QIODevice::ReadOnly)){
			printf("could not open file %s; skipping\n", qPrintable(fn));
			return;
		}

		const QByteArray data = file.readAll();
		QImage img;
		img.loadFromData(data);
		if(img.isNull()){
			printf("could not read image data for %s; skipping...\n",
				   qPrintable(fn));
			return;
		}

		doProcessSlice(img, sliceNum);

		printf("; completed in %.2f secs", timer.s_elapsed());

		mip0volFile_->flush();
	}

private:

	std::map<OmMipChunkCoord, uint64_t> chunkOffsets_;

	uint64_t getChunkOffset(const OmMipChunkCoord& coord)
	{
		if(chunkOffsets_.count(coord)){
			return chunkOffsets_[coord];
		}

		//if chunk was not in map, assume chunk is unallocated...

		const uint64_t offset = vol_->ComputeChunkPtrOffset(coord);

		debugs(io) << "preallocating chunk: " << coord << "\n";
		preallocateChunk(offset);

		return chunkOffsets_[coord] = offset;
	}

	void preallocateChunk(const uint64_t offset)
	{
		static const uint64_t pageSize = 4096;
		for(uint64_t i = 0; i < chunkSizeBytes_; i += pageSize){
			mip0volFile_->seek(offset + i);
			mip0volFile_->putChar(0);
		}
		mip0volFile_->flush();
	}

	void doProcessSlice(const QImage & img, const int sliceNum)
	{
		int tileNum = 0;

		const int z = sliceNum / slicesPerChunk_;

		for(int y = 0; y < mip0dims_.y; ++y) {
			for(int x = 0; x < mip0dims_.x; ++x) {

				const OmMipChunkCoord coord = OmMipChunkCoord(0, x, y, z);
				const DataBbox chunk_bbox = vol_->MipCoordToDataBbox(coord, 0);

				const int startX = chunk_bbox.getMin().x;
				const int startY = chunk_bbox.getMin().y;

				QImage tile = img.copy(startX, startY, sliceWidth_, sliceHeight_);

				const uint64_t chunkOffset = getChunkOffset(coord);
				const uint64_t sliceOffset = (sliceNum % slicesPerChunk_) * sliceSizeBytes_;
				mip0volFile_->seek(chunkOffset+sliceOffset);
				mip0volFile_->write(reinterpret_cast<const char*>(tile.bits()),
									sliceSizeBytes_);

				++tileNum;
				printf("%s %s of %s tiles copied",
					   qPrintable(mMsg),
					   StringHelpers::commaDeliminateNum(tileNum).c_str(),
					   StringHelpers::commaDeliminateNum(totalTilesInSlice_).c_str());
			}
		}
	}
};

#endif
