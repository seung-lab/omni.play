#ifndef MESH_ZIMESHER_H_
#define MESH_ZIMESHER_H_

#include <vector>
#include "volume/omMipChunkCoord.h"
#include "common/omCommon.h"
#include "ziMeshingChunk.h"
#include "utility/omLockedPODs.hpp"

class OmMipMeshManager;

class ziMesher {
public:
  ziMesher(const OmID& segId, OmMipMeshManager* mmManager,
           int rootLevel);

  virtual ~ziMesher(){}

  void mesh();
  void addChunkCoord(const OmMipChunkCoord &c);

  LockedInt64 numOfChunksToProcess;

private:
  OmID               segmentationId_;
  OmMipMeshManager   *mipMeshManager_;
  int                rootMipLevel_;
  std::vector<OmMipChunkCoord> levelZeroChunks_;

  int getQueueSize();
};

#endif
