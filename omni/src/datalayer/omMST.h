#ifndef OM_MST_H
#define OM_MST_H

#include "datalayer/omDataWrapper.h"

class OmSegmentation;
class OmDataReader;
class OmDataPath;

class OmMST {
public:
  OmMST();
  ~OmMST(){}

  void read(OmSegmentation & seg);
  void import(OmSegmentation & seg, const QString fname);
  void FlushDend(OmSegmentation * seg);
  void FlushDendUserEdges(OmSegmentation * seg);

  OmDataWrapperPtr mDend;
  OmDataWrapperPtr mDendValues;
  OmDataWrapperPtr mEdgeDisabledByUser;
  OmDataWrapperPtr mEdgeWasJoined;
  OmDataWrapperPtr mEdgeForceJoin;
  int mDendSize;
  int mDendValuesSize;
  int mDendCount;
  float mDendThreshold;

  bool isValid(){ return valid_; }

private:
  bool valid_;

  bool importDend(OmDataReader * hdf5reader);
  bool importDendValues(OmDataReader * hdf5reader);
  bool setupUserEdges(const int dendValuesSize);

  OmDataPath getDendPath(OmSegmentation & seg);
  OmDataPath getDendValuesPath(OmSegmentation & seg);
  OmDataPath getEdgeDisabledByUserPath(OmSegmentation & seg);
  OmDataPath getEdgeForceJoinPath(OmSegmentation & seg);
};

#endif
