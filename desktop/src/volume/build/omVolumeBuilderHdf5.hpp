#pragma once
#include "precomp.h"

#include "volume/build/omMSTImportHdf5.hpp"
#include "events/events.h"
#include "volume/build/omVolumeBuilderBase.hpp"
#include "volume/build/omDataCopierHdf5.hpp"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"

template <typename VOL>
class OmVolumeBuilderHdf5 : public OmVolumeBuilderBase<VOL> {
 private:
  VOL& vol_;
  const om::common::AffinityGraph aff_;

  std::string fnp_;
  OmDataPath datasetName_;

 public:
  OmVolumeBuilderHdf5(VOL& vol, const QFileInfo& file, const QString& hdf5path,
                      const om::common::AffinityGraph aff =
                          om::common::AffinityGraph::NO_AFFINITY)
      : OmVolumeBuilderBase<VOL>(vol), vol_(vol), aff_(aff) {
    fnp_ = file.filePath().toStdString();

    determineDatasetName(hdf5path);
  }

 protected:
  virtual void importSourceData() {
    OmDataCopierHdf5<VOL> importer(vol_, datasetName_, fnp_, aff_);
    importer.Import();
  }

  virtual Vector3i getMip0Dims() {
    OmHdf5* hdf5 = OmHdf5Manager::Get(fnp_, true);

    hdf5->open();

    const Vector3i dims = hdf5->getChunkedDatasetDims(datasetName_, aff_);

    hdf5->close();

    return dims;
  }

  virtual bool loadDendrogram(OmSegmentation& vol) {
    OmMSTImportHdf5 mstImport(&vol);
    return mstImport.Import(fnp_);
  }

 private:
  void determineDatasetName(const QString& path) {
    OmHdf5* hdf5 = OmHdf5Manager::Get(fnp_, true);

    hdf5->open();

    datasetName_ = OmDataPath(path);
    if (!hdf5->dataset_exists(datasetName_)) {
      datasetName_ = OmDataPaths::getDefaultDatasetName();
    }

    if (!hdf5->dataset_exists(datasetName_)) {
      throw om::IoException("could not find dataset");
    }

    hdf5->close();
  }
};
