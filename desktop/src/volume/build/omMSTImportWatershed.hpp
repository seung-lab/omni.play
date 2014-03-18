#pragma once
#include "precomp.h"

#include "segment/io/omMST.h"
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

    OmVectorInFile<OmMSTImportEdge> edges(fnp);
    edges.Load();

    if (edges.Vector().size() != numEdges) {
      const QString err =
          QString("number of edges mismatch: have %1, but expected %2")
              .arg(edges.Vector().size())
              .arg(numEdges);
      throw om::IoException(err.toStdString());
    }

    vol_->MST()->Import(edges.Vector());

    return true;
  }
};
