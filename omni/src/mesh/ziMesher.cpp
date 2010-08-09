#include "ziMesher.h"
#include "ziMesherManager.h"
#include "boost/shared_ptr.hpp"
#include "zi/base/base.h"
#include <map>
#include <zi/system>
#include "ziMeshingChunk.h"
#include "zi/thread/Thread.h"
#include "zi/thread/ThreadManager.h"

using boost::shared_ptr;
using namespace zi::Threads;

void ziMesher::addChunkCoord(const OmMipChunkCoord &c) {
  levelZeroChunks_.push_back(c);
}

ziMesher::ziMesher(const OmId &segId, OmMipMeshManager *mmManager,
                   int rootLevel)
  : segmentationId_(segId),
    mipMeshManager_(mmManager),
    rootMipLevel_(rootLevel),
    levelZeroChunks_()
{

}

void ziMesher::mesh() {
  std::map<OmMipChunkCoord, shared_ptr<GrowingMeshes> > allChunks_;
  std::vector<shared_ptr<ziMeshingChunk> > workers;

  FOR_EACH(it, levelZeroChunks_) {

    double error = 1e-5;
    OmMipChunkCoord c = *it;
    shared_ptr<ziMeshingChunk> worker(new ziMeshingChunk(segmentationId_, c,
                                                         mipMeshManager_));
    workers.push_back(worker);
    allChunks_[c] = shared_ptr<GrowingMeshes>(new GrowingMeshes(c));
    worker->deliverTo(allChunks_[c], error);
    error *= 2;

    do {
      c = c.ParentCoord();
      if (allChunks_.find(c) == allChunks_.end()) {
        allChunks_[c] = shared_ptr<GrowingMeshes>(new GrowingMeshes(c));
      } else {
        allChunks_[c]->requireDelivery();
      }
      worker->deliverTo(allChunks_[c], error);
      error *= 2;
    } while (c.getLevel() < rootMipLevel_);

  }

  shared_ptr<ThreadFactory> factory(new ThreadFactory());
  factory->setDetached(false);
  shared_ptr<ThreadManager> manager =
    shared_ptr<ThreadManager>(new ThreadManager(factory, getQueueSize()));

  manager->start();

  FOR_EACH(it, workers)
    manager->addTask(*it);

  manager->join();
}

int ziMesher::getQueueSize() {
  //TODO: retrieve from omLocalPreferences? (purcaro)
  //Update by aleks: this actually shouldn't be too big, since
  // each thread eats a lot of memory (pre loads the data)
  int idealNum = zi::System::GetTotalGB() / 2;
  if( idealNum < 2){
    idealNum = 2;
  }
  printf("ziMesher: will use %d threads\n", idealNum);
  return idealNum;
}
