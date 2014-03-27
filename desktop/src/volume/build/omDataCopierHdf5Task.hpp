#pragma once
#include "precomp.h"

#include "chunks/omChunk.h"
#include "common/common.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "threads/omTaskManager.hpp"
#include "utility/image/omImage.hpp"
#include "utility/omSimpleProgress.hpp"
#include "volume/omVolumeTypes.hpp"

template <typename VOL>
class OmDataCopierHdf5Task : public zi::runnable {
 private:
  VOL* const vol_;
  const OmDataPath path_;
  const om::common::AffinityGraph aff_;

  const Vector3i volSize_;
  OmHdf5* const hdf5reader_;
  const QString mip0fnp_;
  const om::coords::Chunk coord_;

  OmSimpleProgress* prog_;

  OmChunk* chunk_;
  bool resizedChunk_;

 public:
  OmDataCopierHdf5Task(VOL* vol, const OmDataPath& path,
                       const om::common::AffinityGraph aff,
                       const Vector3i volSize, OmHdf5* const hdf5reader,
                       const QString mip0fnp, const om::coords::Chunk& coord,
                       OmSimpleProgress* prog)
      : vol_(vol),
        path_(path),
        aff_(aff),
        volSize_(volSize),
        hdf5reader_(hdf5reader),
        mip0fnp_(mip0fnp),
        coord_(coord),
        prog_(prog),
        resizedChunk_(false) {
    chunk_ = vol_->GetChunk(coord_);
  }

  virtual ~OmDataCopierHdf5Task() {}

  void run() {
    OmTimer timer;

    copyIntoChunk();

    std::ostringstream stm;
    stm << "copied ";
    if (resizedChunk_) {
      stm << "and resized ";
    }
    stm << "HDF5 chunk: " << coord_ << " in " << timer.s_elapsed() << " secs ";

    prog_->DidOne(stm.str());
  }

 private:
  template <typename T>
  OmDataWrapperPtr doResizePartialChunk(
      OmDataWrapperPtr data, const om::coords::DataBbox& chunkExtent,
      const om::coords::DataBbox& dataExtent) {
    resizedChunk_ = true;

    const Vector3i dataSize = dataExtent.getDimensions();

    // weirdness w/ hdf5 and/or boost::multi_arry requires flipping x/z
    // TODO: figure out!
    OmImage<T, 3> partialChunk(OmExtents[dataSize.z][dataSize.y][dataSize.x],
                               data->getPtr<T>());

    const Vector3i chunkSize = chunkExtent.getDimensions();

    partialChunk.resize(chunkSize);

    return om::ptrs::Wrap(partialChunk.getMallocCopyOfData());
  }

  OmDataWrapperPtr resizePartialChunk(OmDataWrapperPtr data,
                                      const om::coords::DataBbox& chunkExtent,
                                      const om::coords::DataBbox& dataExtent) {
    switch (data->getVolDataType().index()) {
      case om::common::DataType::INT8:
        return doResizePartialChunk<int8_t>(data, chunkExtent, dataExtent);
      case om::common::DataType::UINT8:
        return doResizePartialChunk<uint8_t>(data, chunkExtent, dataExtent);
      case om::common::DataType::INT32:
        return doResizePartialChunk<int32_t>(data, chunkExtent, dataExtent);
      case om::common::DataType::UINT32:
        return doResizePartialChunk<uint32_t>(data, chunkExtent, dataExtent);
      case om::common::DataType::FLOAT:
        return doResizePartialChunk<float>(data, chunkExtent, dataExtent);
      case om::common::DataType::UNKNOWN:
      default:
        throw om::IoException("unknown data type");
    }
  }

  void copyIntoChunk() {
    OmDataWrapperPtr data = getChunkData();

    const uint64_t chunkOffset =
        OmChunkOffset::ComputeChunkPtrOffsetBytes(*vol_, coord_);

    QFile file(mip0fnp_);
    if (!file.open(QIODevice::ReadWrite)) {
      throw om::IoException("could not open file");
    }

    file.seek(chunkOffset);
    file.write(data->getPtr<const char>(), chunk_->Mipping().NumBytes());
  }

  OmDataWrapperPtr getChunkData() {
    // get chunk data bbox
    const om::coords::DataBbox& chunkExtent = chunk_->Mipping().Extent();

    const om::coords::DataBbox volExtent(
        om::coords::Data(Vector3i::ZERO, vol_->Coords(), chunk_->GetLevel()),
        om::coords::Data(volSize_, vol_->Coords(), chunk_->GetLevel()));

    // if data extent contains given extent, read full chunk
    if (volExtent.contains(chunkExtent)) {
      return hdf5reader_->readChunk(path_, chunkExtent, aff_);
    }

    // else, read what we can, and resize

    om::coords::DataBbox intersect_extent = chunkExtent;
    intersect_extent.intersect(volExtent);

    if (intersect_extent.isEmpty()) {
      throw om::IoException("should not have happened");
    }

    OmDataWrapperPtr partialChunk =
        hdf5reader_->readChunk(path_, intersect_extent, aff_);

    return resizePartialChunk(partialChunk, chunkExtent, intersect_extent);
  }
};
