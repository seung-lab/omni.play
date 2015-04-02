#pragma once
#include "precomp.h"

#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"
#include "segment/omSegments.h"
#include "segment/types.hpp"
#include "volume/omSegmentation.h"

class OmMSTImportHdf5 {
 private:
  OmSegmentation* const vol_;
  std::vector<om::segment::ImportEdge> edges_;

 public:
  OmMSTImportHdf5(OmSegmentation* vol) : vol_(vol) {}

  bool Import(const std::string& fname) {
    if (!readFromHDF5(fname)) {
      return false;
    }

    checkSizes();

    process();

    checkSegmentIDs();

    auto& mst = vol_->MST();

    mst.resize(edges_.size());
    for (size_t i = 0; i < edges_.size(); ++i) {
      mst[i].number = i;
      mst[i].node1ID = edges_[i].seg1;
      mst[i].node2ID = edges_[i].seg2;
      mst[i].threshold = edges_[i].threshold;
      mst[i].userJoin = 0;
      mst[i].userSplit = 0;
      mst[i].wasJoined = 0;
    }
    mst.flush();

    return true;
  }

 private:
  OmDataWrapperPtr dend_;
  Vector3i dendSizes_;  // example:  2 x 13,672

  OmDataWrapperPtr dendValues_;
  Vector3i dendValuesSizes_;  // example:  13,672 x 0

  bool readFromHDF5(const std::string& fname) {
    OmHdf5* hdf5 = OmHdf5Manager::Get(fname, true);
    hdf5->open();

    if (!importDend(hdf5)) {
      return false;
    }

    if (!importDendValues(hdf5)) {
      return false;
    }

    hdf5->close();

    return true;
  }

  void checkSizes() {
    if (dendSizes_.y != dendValuesSizes_.x) {
      throw om::IoException("MST size mismatch");
    }
  }

  void process() {
    const uint32_t numEdges = edges_.size();

    uint32_t const* const nodes = dend_->getPtr<uint32_t>();
    float const* const thresholds = dendValues_->getPtr<float>();

    for (uint32_t i = 0; i < numEdges; ++i) {
      edges_[i].seg1 = nodes[i];
      edges_[i].seg2 = nodes[i + numEdges];
      edges_[i].threshold = thresholds[i];
    }
  }

  bool importDend(OmHdf5* hdf5) {
    OmDataPath datasetName("dend");

    if (!hdf5->dataset_exists(datasetName)) {
      log_infos << "no dendrogram dataset found";
      return false;
    }

    dendSizes_ = hdf5->getDatasetDims(datasetName);

    const int numEdges = dendSizes_.y;
    edges_.resize(numEdges);

    int numBytes;
    dend_ = hdf5->readDataset(datasetName, &numBytes);

    log_info("\tdendrogram is %s x %s (%s bytes)",
             om::string::humanizeNum(dendSizes_.x).c_str(),
             om::string::humanizeNum(dendSizes_.y).c_str(),
             om::string::humanizeNum(numBytes).c_str());

    return true;
  }

  bool importDendValues(OmHdf5* hdf5) {
    OmDataPath datasetName("dendValues");

    if (!hdf5->dataset_exists(datasetName)) {
      log_infos << "no dendrogram values dataset found";
      return false;
    }

    dendValuesSizes_ = hdf5->getDatasetDims(datasetName);

    int numBytes;
    dendValues_ = hdf5->readDataset(datasetName, &numBytes);

    log_info("\tdendrogram values is %s x %s (%s bytes)",
             om::string::humanizeNum(dendValuesSizes_.x).c_str(),
             om::string::humanizeNum(dendValuesSizes_.y).c_str(),
             om::string::humanizeNum(numBytes).c_str());

    return true;
  }

  void checkSegmentIDs() {
    const uint32_t size = edges_.size();

    std::vector<om::segment::ImportEdge> valid;
    valid.reserve(size);

    auto& segments = vol_->Segments();

    for (uint32_t i = 0; i < size; ++i) {
      if (segments.IsSegmentValid(edges_[i].seg1)
          && segments.IsSegmentValid(edges_[i].seg2)) {

            valid.push_back(edges_[i]);
            continue;
      }


      log_infos << "MST edge import: skipping: " << edges_[i];
    }

    const uint32_t oldSize = edges_.size();
    const uint32_t newSize = valid.size();
    const uint32_t diff = oldSize - newSize;

    edges_.swap(valid);

    if (!diff) {
      log_infos << "checked MST: all edges good!";

    } else {
      log_infos << "checked MST: "
                << "removed " << diff << " invalid edges; " << newSize
                << " edges left";
    }
  }
};
