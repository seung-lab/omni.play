#pragma once
#include "precomp.h"

#include "chunks/omChunkDataImpl.hpp"
#include "chunks/omSegChunk.h"
#include "chunks/omSegChunkDataInterface.hpp"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "segment/omSegments.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

/**
 * Export MIP 0 to HDF5
 */
class OmExportVolToHdf5 {
 public:
  static void Export(OmChannel& chan, const QString& fnp) {
    OmExportVolToHdf5 e;
    e.exportVol(&chan, fnp, false);
  }

  static void Export(OmSegmentation* seg, const QString& fnp,
                     const bool rerootSegments) {
    OmExportVolToHdf5 e;
    e.exportVol(seg, fnp, rerootSegments);
  }

 private:
  OmExportVolToHdf5() {}

  ~OmExportVolToHdf5() {}

  template <typename VOL>
  void exportVol(VOL* vol, const QString& fileNameAndPath,
                 const bool rerootSegments) {
    log_infos << "starting export...";

    OmHdf5* hdfExport = OmHdf5Manager::Get(fileNameAndPath, false);
    OmDataPath fpath("main");

    if (!QFile::exists(fileNameAndPath)) {
      hdfExport->create();
      hdfExport->open();
      const Vector3i chunkSize = vol->Coords().ChunkDimensions();
      const Vector3i rounded_data_dims =
          vol->Coords().DimsRoundedToNearestChunk(0);
      hdfExport->allocateChunkedDataset(fpath, rounded_data_dims, chunkSize,
                                        vol->getVolDataType());
    } else {
      hdfExport->open();
    }

    std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
        vol->GetMipChunkCoords(0);

    FOR_EACH(iter, *coordsPtr) {
      const om::coords::Chunk& coord = *iter;

      OmDataWrapperPtr data = exportChunk(vol, coord, rerootSegments);
      om::coords::DataBbox chunk_data_bbox = coord.BoundingBox(vol->Coords());

      hdfExport->writeChunk(OmDataPaths::getDefaultDatasetName(),
                            chunk_data_bbox, data);
    }

    hdfExport->close();

    log_infos << "export done!";
  }

  OmDataWrapperPtr exportChunk(OmChannel* vol, const om::coords::Chunk& coord,
                               const bool) {
    OmChunk* chunk = vol->GetChunk(coord);
    return chunk->Data()->CopyOutChunkData();
  }

  OmDataWrapperPtr exportChunk(OmSegmentation* vol,
                               const om::coords::Chunk& coord,
                               const bool rerootSegments) {
    log_infos << "\r\texporting " << coord << std::flush;

    OmSegChunk* chunk = vol->GetChunk(coord);

    std::shared_ptr<uint32_t> rawDataPtr =
        chunk->SegData()->GetCopyOfChunkDataAsUint32();

    if (rerootSegments) {
      uint32_t* rawData = rawDataPtr.get();
      auto& segments = vol->Segments();

      for (uint32_t i = 0; i < chunk->Mipping().NumVoxels(); ++i) {
        if (0 != rawData[i]) {
          rawData[i] = segments.FindRootID(rawData[i]);
        }
      }
    }

    return om::ptrs::Wrap(rawDataPtr);
  }
};
