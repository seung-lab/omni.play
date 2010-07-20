#ifndef MESH_ZIMESHER_H_
#define MESH_ZIMESHER_H_

#include <vector>
#include "volume/omMipChunkCoord.h"
#include "omMipMeshManager.h"
#include "common/omCommon.h"
#include "ziMeshingChunk.h"

class ziMesher {
public:
  ziMesher(const OmId &segId, OmMipMeshManager *mmManager,
           int rootLevel);
  virtual ~ziMesher() {}

  void mesh();
  void addChunkCoord(const OmMipChunkCoord &c);

private:
  OmId               segmentationId_;
  OmMipMeshManager   *mipMeshManager_;
  int                rootMipLevel_;
  std::vector<OmMipChunkCoord> levelZeroChunks_;

  int getQueueSize();
};

#endif
