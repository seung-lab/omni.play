#pragma once
#include "precomp.h"

#include "common/common.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "threads/taskManager.hpp"
#include "utility/image/omImage.hpp"
#include "utility/omSimpleProgress.hpp"
#include "volume/omVolumeTypes.hpp"

class OmDataCopierHdf5Task : public zi::runnable {
private:
  unsigned int bytesPerPlane_;
  const OmDataPath path_;
  const om::common::AffinityGraph aff_;

  const Vector3i volSize_;
  OmHdf5* const hdf5reader_;
  const QString mip0fnp_;
  const om::coords::Chunk coord_;
  const om::coords::VolumeSystem vol_coords_;

  OmSimpleProgress* prog_;

  bool resizedChunk_;

public:
  OmDataCopierHdf5Task(unsigned int bytesPerPlane, om::coords::VolumeSystem vol_coords , const OmDataPath& path,
                       const om::common::AffinityGraph aff,
                       const Vector3i volSize, OmHdf5* const hdf5reader,
                       const QString mip0fnp, const om::coords::Chunk& coord,
                       OmSimpleProgress* prog)
    : bytesPerPlane_(bytesPerPlane),
      path_(path),
      aff_(aff),
      volSize_(volSize),
      hdf5reader_(hdf5reader),
      mip0fnp_(mip0fnp),
      coord_(coord),
      vol_coords_(vol_coords),
      prog_(prog){}

  virtual ~OmDataCopierHdf5Task() {}

  void run() {
    OmTimer timer;

    copyIntoChunk();

    std::ostringstream stm;
    stm << "copied HDF5 chunk: " << coord_ << " in " << timer.s_elapsed() << " secs ";
    prog_->DidOne(stm.str());
  }

private:
  void copyIntoChunk() {

    const uint64_t chunkOffset = coord_.PtrOffset(vol_coords_, bytesPerPlane_);
    QFile file(mip0fnp_);
    if (!file.open(QIODevice::ReadWrite)) {
      throw om::IoException("could not open file");
    }
    file.seek(chunkOffset);

    const om::coords::DataBbox volExtent(
          om::coords::Data(Vector3i::ZERO, vol_coords_, coord_.mipLevel()),
          om::coords::Data(volSize_-1, vol_coords_, coord_.mipLevel()));

    const om::coords::DataBbox& chunkExtent = coord_.BoundingBox(vol_coords_);
    om::coords::DataBbox intersect_extent = chunkExtent;
    intersect_extent.intersect(volExtent);

    if (intersect_extent.isEmpty()) {
      throw om::IoException("the intersection between the volume and the chunk is null");
    }

    auto dims = intersect_extent.getUnitDimensions();
    auto numBytes =  dims[2] * bytesPerPlane_;
    OmDataWrapperPtr data = hdf5reader_->readChunk(path_, intersect_extent, aff_);

    switch (data->getVolDataType().index()) {
      case om::common::DataType::INT8:
        data = doResizePartialChunk<int8_t>(data, chunkExtent, intersect_extent);
        break;
      case om::common::DataType::UINT8:
        data = doResizePartialChunk<uint8_t>(data, chunkExtent, intersect_extent);
        break;
      case om::common::DataType::INT32:
        data = doResizePartialChunk<int32_t>(data, chunkExtent, intersect_extent);
        break;
      case om::common::DataType::UINT32:
        data = doResizePartialChunk<uint32_t>(data, chunkExtent, intersect_extent);
        break;
      case om::common::DataType::FLOAT:
        data = doResizePartialChunk<float>(data, chunkExtent, intersect_extent);
        break;
      case om::common::DataType::UNKNOWN:
      default:
        throw om::IoException("unknown data type" + std::to_string(data->getVolDataType().index()));
    }

    file.write(data->getPtr<const char>(), numBytes);
  }
  template <typename T>   OmDataWrapperPtr doResizePartialChunk(
    OmDataWrapperPtr data, const om::coords::DataBbox& chunkExtent, const om::coords::DataBbox& dataExtent) {

    const Vector3i dataSize = dataExtent.getUnitDimensions();
    OmImage<T, 3> partialChunk(OmExtents[dataSize.z][dataSize.y][dataSize.x],	data->getPtr<T>());

    const Vector3i chunkSize = chunkExtent.getUnitDimensions();
    partialChunk.resize(chunkSize);

    return  om::ptrs::Wrap(partialChunk.getMallocCopyOfData());
  }
};
