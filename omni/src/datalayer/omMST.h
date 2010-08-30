#ifndef OM_MST_H
#define OM_MST_H

#include "datalayer/omDataWrapper.h"

class OmSegmentation;
class OmIDataReader;
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
  boost::shared_ptr<uint8_t> mEdgeWasJoined;
  OmDataWrapperPtr mEdgeForceJoin;
  int mDendSize;
  int mDendValuesSize;
  int mDendCount;
  float mDendThreshold;

  bool isValid(){ return valid_; }

private:
  bool valid_;

  bool importDend(OmIDataReader*);
  bool importDendValues(OmIDataReader*);
  bool setupUserEdges(const int);

  OmDataPath getDendPath(OmSegmentation&);
  OmDataPath getDendValuesPath(OmSegmentation&);
  OmDataPath getEdgeDisabledByUserPath(OmSegmentation&);
  OmDataPath getEdgeForceJoinPath(OmSegmentation&);
};

#endif
