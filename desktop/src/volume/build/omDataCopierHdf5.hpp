#pragma once
#include "precomp.h"

#include "common/common.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "chunks/omChunk.h"
#include "utility/omStringHelpers.h"
#include "threads/omTaskManager.hpp"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "volume/build/omDataCopierBase.hpp"
#include "volume/build/omDataCopierHdf5Task.hpp"

template <typename VOL>
class OmDataCopierHdf5 : public OmDataCopierBase<VOL> {
 private:
  VOL& vol_;
  const OmDataPath path_;
  const om::common::AffinityGraph aff_;

  Vector3i volSize_;
  OmHdf5* hdf5reader_;
  QString mip0fnp_;

 public:
  OmDataCopierHdf5(VOL& vol, const OmDataPath& path, const std::string fnp,
                   const om::common::AffinityGraph aff)
      : OmDataCopierBase<VOL>(&vol), vol_(vol), path_(path), aff_(aff) {
    hdf5reader_ = OmHdf5Manager::Get(fnp, true);
    hdf5reader_->open();

    log_infos << "importHDF5: source path is: " << path_;

    volSize_ = hdf5reader_->getChunkedDatasetDims(path_, aff_);
    log_infos << "importHDF5: source vol dims: " << volSize_;
  }

  ~OmDataCopierHdf5() { hdf5reader_->close(); }

 private:
  virtual void doImport() {
    allocateData(determineDataType());

    OmTimer timer;
    log_infos << "copying in HDF5 data...";

    OmThreadPool threadPool;
    threadPool.start(3);

    std::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
        vol_.GetMipChunkCoords(0);

    OmSimpleProgress prog(coordsPtr->size(), "HDF5 chunk copy");

    FOR_EACH(iter, *coordsPtr) {
      const om::coords::Chunk& coord = *iter;

      std::shared_ptr<OmDataCopierHdf5Task<VOL> > task =
          std::make_shared<OmDataCopierHdf5Task<VOL> >(&vol_, path_, aff_,
                                                       volSize_, hdf5reader_,
                                                       mip0fnp_, coord, &prog);
      threadPool.push_back(task);
    }

    threadPool.join();

    log_infos << "HDF5 data copy done in " << timer.s_elapsed() << " secs";
  }

  void allocateData(const om::common::DataType type) {
    std::vector<std::shared_ptr<QFile> > volFiles =
        OmVolumeAllocater::AllocateData(&vol_, type);

    mip0fnp_ = QFileInfo(*volFiles[0]).absoluteFilePath();
  }

  om::common::DataType determineDataType() {
    OmDataWrapperPtr data = hdf5reader_->GetChunkDataType(path_);
    return data->getVolDataType();
  }
};
