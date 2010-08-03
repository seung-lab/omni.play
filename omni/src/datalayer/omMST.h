#ifndef OM_MST_H
#define OM_MST_H

#include "datalayer/omDataWrapper.h"

class OmSegmentation;
class OmDataReader;

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

  bool isValid(){ return valid; }

private:
  bool valid;

  void convertToEdgeList( OmSegmentation & seg,
			  quint32 * dend,
			  float * dendValues,
			  const int numDendRows );

  bool importDend(OmDataReader * hdf5reader);
  bool importDendValues(OmDataReader * hdf5reader);
  bool setupUserEdges(const int dendValuesSize);
};

#endif
