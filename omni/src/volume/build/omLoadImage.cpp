#include "utility/stringHelpers.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include "volume/build/omLoadImage.h"
#include "volume/omMipVolume.h"
#include "utility/omTimer.h"

template <typename T>
OmLoadImage<T>::OmLoadImage(OmVolumeImporter<T>* importer, T* p)
	: importer(importer)
	, mMipVolume(p)
	, m_leaf_mip_dims(mMipVolume->MipLevelDimensionsInMipChunks(0))
	, mTotalNumImages(mMipVolume->mSourceFilenamesAndPaths.size())
{
}

template <typename T>
void OmLoadImage<T>::processSlice(const QString & fn, const int sliceNum )
{
	OmTimer slice_timer;
	slice_timer.start();

	mMsg = QString("\r(%1 of %2) loading image %3...").arg(sliceNum+1).arg(mTotalNumImages).arg(fn);
	printf("\n%s", qPrintable(fn));
	fflush(stdout);

	QImage img(fn);
	if(img.isNull()){
		printf("could not open %s\n", qPrintable(fn));
		assert(0 && "bad image?");
	}

	doProcessSlice(img, sliceNum);

	slice_timer.stop();
	printf("completed in %.2f secs", slice_timer.s_elapsed());
}

template <typename T>
void OmLoadImage<T>::doProcessSlice(const QImage & img, const int sliceNum)
{
	int chunkNum=0;

	const int totalChunksInSlice = m_leaf_mip_dims.x * m_leaf_mip_dims.y;

	const int z = floor(sliceNum/128);
	for(int y = 0; y < m_leaf_mip_dims.y; ++y) {
		for(int x = 0; x < m_leaf_mip_dims.x; ++x) {

			const OmMipChunkCoord chunk_coord = OmMipChunkCoord(0, x, y, z);
			const DataBbox chunk_bbox = mMipVolume->MipCoordToDataBbox(chunk_coord, 0);

			const int startX = chunk_bbox.getMin().x;
			const int startY = chunk_bbox.getMin().y;
			const int h = 128;
			const int w = 128;

			OmMipChunkPtr chunk;
			mMipVolume->GetChunk(chunk, chunk_coord);

			QImage tile = img.copy(startX,startY,w,h);

			chunk->copyInTile(sliceNum, tile.bits());

			++chunkNum;
			QString numTiles = QString("%1 of %2 tiles copied...")
				.arg(StringHelpers::commaDeliminateNumber(chunkNum))
				.arg(StringHelpers::commaDeliminateNumber(totalChunksInSlice));
			printf("%s", qPrintable(QString(mMsg+numTiles)));
		}
	}
}
