#ifndef OM_EXPORT_VOLUME_TO_HDF5_HPP
#define OM_EXPORT_VOLUME_TO_HDF5_HPP

#include "chunks/omSegChunk.h"
#include "chunks/omChunkCoord.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCache.h"

/**
 *	Export MIP 0 to HDF5
 */
class OmExportVolToHdf5 {
public:
	static void Export(OmChannel& chan, const QString& fnp)
	{
		OmExportVolToHdf5 e;
		e.exportVol(&chan, fnp, false);
	}

	static void Export(OmSegmentation* seg, const QString& fnp,
					   const bool rerootSegments)
	{
		OmExportVolToHdf5 e;
		e.exportVol(seg, fnp, rerootSegments);
	}

private:
	OmExportVolToHdf5()
	{}

	virtual ~OmExportVolToHdf5()
	{}

	template <typename VOL>
	void exportVol(VOL* vol, const QString& fileNameAndPath,
				   const bool rerootSegments)
	{
		printf("starting export...\n");

		OmHdf5* hdfExport = OmHdf5Manager::Get(fileNameAndPath, false);
		OmDataPath fpath("main");

		if(!QFile::exists(fileNameAndPath)){
			hdfExport->create();
			hdfExport->open();
			const Vector3i full = vol->Coords().MipLevelDataDimensions(0);
			const Vector3i rounded_data_dims =
				vol->Coords().getDimsRoundedToNearestChunk(0);
			hdfExport->allocateChunkedDataset(fpath,
											  rounded_data_dims,
											  full,
											  vol->getVolDataType());
		} else {
			hdfExport->open();
		}

		boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
			vol->GetMipChunkCoords(0);

		FOR_EACH(iter, *coordsPtr){
			const OmChunkCoord& coord = *iter;

			OmDataWrapperPtr data = exportChunk(vol, coord, rerootSegments);
			const DataBbox chunk_data_bbox =
				vol->Coords().MipCoordToDataBbox(coord, 0);

			hdfExport->writeChunk(OmDataPaths::getDefaultDatasetName(),
								  chunk_data_bbox,
								  data);
		}

		hdfExport->close();

		printf("\nexport done!\n");
	}

	OmDataWrapperPtr exportChunk(OmChannel* vol, const OmChunkCoord& coord,
								 const bool)
	{
		OmChunkPtr chunk;
		vol->GetChunk(chunk, coord);
		return chunk->Data()->CopyOutChunkData();
	}

	OmDataWrapperPtr exportChunk(OmSegmentation* vol,
								 const OmChunkCoord& coord,
								 const bool rerootSegments)
	{
		std::cout << "\r\texporting " << coord << std::flush;

		OmSegChunkPtr chunk;
		vol->GetChunk(chunk, coord);

		boost::shared_ptr<uint32_t> rawDataPtr =
			chunk->SegData()->GetCopyOfChunkDataAsUint32();

		if(rerootSegments){
			uint32_t* rawData = rawDataPtr.get();
			OmSegmentCache* segmentCache = vol->SegmentCache();

			for(uint32_t i = 0; i < chunk->Mipping().NumVoxels(); ++i){
				if( 0 != rawData[i]) {
					rawData[i] = segmentCache->findRootID(rawData[i]);
				}
			}
		}

		return OmDataWrapperFactory::produce(rawDataPtr);
	}
};
#endif
