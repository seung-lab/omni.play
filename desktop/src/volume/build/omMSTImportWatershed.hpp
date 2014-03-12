#pragma once

#include "volume/omSegmentation.h"
#include "datalayer/fs/omVecInFile.hpp"

class OmMSTImportWatershed {
 private:
  OmSegmentation* const vol_;

 public:
  OmMSTImportWatershed(OmSegmentation* vol) : vol_(vol) {}

  bool Import(const QString& fnp, const int bitsPerNode,
              const uint32_t numEdges) {
    if (32 != bitsPerNode) {
      throw om::IoException("only know how to process 32-bit node values");
    }

    OmVectorInFile<om::segment::ImportEdge> edges(fnp);
    edges.Load();

    if (edges.Vector().size() != numEdges) {
      const QString err =
          QString("number of edges mismatch: have %1, but expected %2")
              .arg(edges.Vector().size())
              .arg(numEdges);
      throw om::IoException(err.toStdString());
    }

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
};
