#pragma once

#include "datalayer/omDataWrapper.h"

class OmSegmentation;
class OmMSTEdge;

class OmMSTold {
 private:
  OmSegmentation* const segmentation_;

 public:
  OmMSTold(OmSegmentation* segmentation) : segmentation_(segmentation) {}

  void ReadOld();
  bool IsPopulated();
  void MoveData(OmMSTEdge* edges, const uint32_t numEdges);

  OmDataWrapperPtr mDend;
  OmDataWrapperPtr mDendValues;
  OmDataWrapperPtr mEdgeDisabledByUser;
  OmDataWrapperPtr mEdgeForceJoin;

 private:
  OmDataPath getDendPath();
  OmDataPath getDendValuesPath();
  OmDataPath getEdgeDisabledByUserPath();
  OmDataPath getEdgeForceJoinPath();
};
