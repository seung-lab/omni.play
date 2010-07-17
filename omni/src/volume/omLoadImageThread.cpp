#include "utility/stringHelpers.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omLoadImageThread.h"
#include "volume/omMipVolume.h"
#include "utility/omTimer.h"

OmLoadImageThread::OmLoadImageThread(OmMipVolume * p)
	: mMipVolume(p)
	, m_leaf_mip_dims(mMipVolume->MipLevelDimensionsInMipChunks(0))
	, m_numberOfBytes(mMipVolume->GetBytesPerSample())
{
}

void OmLoadImageThread::run()
{
	while(1){
		const std::pair<int,QString> f = mMipVolume->getNextImgToProcess();
		if(-1==f.first){
			return;
		}
		processSlice(f.second, f.first);
	}
}

void OmLoadImageThread::processSlice(const QString & fn, const int sliceNum )
{
	OmTimer slice_timer;
	slice_timer.start();

	printf("loading image %s...\n", qPrintable(fn));

	QImage img(fn);
	if(img.isNull()){
		printf("could not open %s\n", qPrintable(fn));
		assert(0 && "bad image?");
	}		

	doProcessSlice(img, sliceNum);

	slice_timer.stop();
	printf("completed %s in %.6f secs\n", qPrintable(fn), slice_timer.s_elapsed());
}

void OmLoadImageThread::doProcessSlice(const QImage & img, const int sliceNum)
{
	//const int totalChunksInSlice = m_leaf_mip_dims.x * m_leaf_mip_dims.y;
	int chunkNum=0;

	const int z = floor(sliceNum/128);
	for(int y = 0; y < m_leaf_mip_dims.y; ++y) {
		for(int x = 0; x < m_leaf_mip_dims.x; ++x) {
			
			const OmMipChunkCoord chunk_coord = OmMipChunkCoord(0, x, y, z);
			const DataBbox chunk_data_bbox = mMipVolume->MipCoordToDataBbox(chunk_coord, 0);

			mMipVolume->addToChunkCoords(chunk_coord);

			const int startX = chunk_data_bbox.getMin().x;
			const int startY = chunk_data_bbox.getMin().y;
			const int h = 128;
			const int w = 128;
			
			QExplicitlySharedDataPointer<OmMipChunk> chunk = 
				QExplicitlySharedDataPointer<OmMipChunk>();
			mMipVolume->GetChunk(chunk, chunk_coord);

			QImage tile = img.copy(startX,startY,w,h);
			const int advance = (128*128*(sliceNum%128));

			if(4 == m_numberOfBytes){
				assert(0);
				OmDataWrapperPtr dataPtr = chunk->RawReadChunkDataUINT32();
				quint32* data = dataPtr->getQuint32Ptr();

				QRgb* bits32 = (QRgb*)tile.bits();
				for(int i=0; i<128*128; ++i){
					data[advance+i] = bits32[i];
				}
			} else{
				OmDataWrapperPtr dataPtr = chunk->RawReadChunkDataUCHARmapped();
				unsigned char* data = dataPtr->getQuint8Ptr();
				uchar* bits8 = tile.bits();
				
				memcpy(data+advance, bits8, 128*128*1);
			}
			++chunkNum;

			/*
			printf("\r\t%s of %s tiles copied...", 
			       qPrintable(StringHelpers::commaDeliminateNumber(chunkNum)), 
			       qPrintable(StringHelpers::commaDeliminateNumber(totalChunksInSlice)));
			*/
		}
	}
}
