#pragma once
#include "precomp.h"

#include "volume/omSegmentation.h"

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
    om::file::path path(fnp.toStdString());
    if (!om::file::exists(path)) {
      return false;
    }
    std::vector<om::segment::ImportEdge> edges;
    om::file::readAll(fnp.toStdString(), edges);

    if (edges.size() != numEdges) {
      const QString err =
          QString("number of edges mismatch: have %1, but expected %2")
              .arg(edges.size())
              .arg(numEdges);
      throw om::IoException(err.toStdString());
    }

    auto& mst = vol_->MST();

    mst.resize(edges.size());
    for (size_t i = 0; i < edges.size(); ++i) {
      mst[i].number = i;
      mst[i].node1ID = edges[i].seg1;
      mst[i].node2ID = edges[i].seg2;
      mst[i].threshold = edges[i].threshold;
      mst[i].userJoin = 0;
      mst[i].userSplit = 0;
      mst[i].wasJoined = 0;
    }
    mst.flush();

    return true;
  }
};
