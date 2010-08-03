#ifndef  ZIMESHINGCHUNK_H_
#define  ZIMESHINGCHUNK_H_

#include "zi/base/omni.h"
#include <zi/threads>
#include "mesh/omMipMeshManager.h"
#include "volume/omMipChunk.h"
#include "volume/omMipVolume.h"
#include "volume/omMipChunkCoord.h"
#include "boost/shared_ptr.hpp"
#include "ziMesherManager.h"
#include <vector>
#include <utility>
#include <vtkImageData.h>

class ziMesher;

class ziMeshingChunk : public zi::Runnable {
public:
  ziMeshingChunk(int segId, OmMipChunkCoord c,
		 OmMipMeshManager *mmm, ziMesher *mesher);
  virtual ~ziMeshingChunk() {}
  void run();
  void deliverTo(boost::shared_ptr<GrowingMeshes> level,
                 double maxError);
private:

  void loadChunk_();
  int                           segmentationId_;
  OmMipChunkCoord               mipCoord_;
  OmMipMeshManager              *mipMeshManager_;
  ziMesher                      *mesher_;
  OmMipChunkPtr                 chunk_;
  vtkImageData                  *pImageData_;
  AxisAlignedBoundingBox<int>   srcBbox_;
  AxisAlignedBoundingBox<float> dstBbox_;
  zi::vector::Vec3d             scale_;
  zi::vector::Vec3d             translate_;
  std::vector<std::pair<boost::shared_ptr<GrowingMeshes>, double> > toDeliver_;
};

#endif
