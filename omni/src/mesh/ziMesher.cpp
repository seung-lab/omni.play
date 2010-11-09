#include "mesh/omMeshParams.hpp"
#include "mesh/omMipMeshManager.h"
#include "zi/base/base.h"
#include "ziMesher.h"
#include "ziMesherManager.h"
#include "ziMeshingChunk.h"

#include <map>
#include "boost/shared_ptr.hpp"

#include "zi/omThreads.h"
#include <zi/system.hpp>

using boost::shared_ptr;

void ziMesher::addChunkCoord(const OmMipChunkCoord &c) {
	levelZeroChunks_.push_back(c);
}

ziMesher::ziMesher(const OmID &segId, OmMipMeshManager *mmManager,
                   int rootLevel)
	: segmentationId_(segId),
	  mipMeshManager_(mmManager),
	  rootMipLevel_(rootLevel),
	  levelZeroChunks_()
{
}

void ziMesher::mesh() {
	numOfChunksToProcess.set(levelZeroChunks_.size());
	std::map<OmMipChunkCoord, shared_ptr<GrowingMeshes> > allChunks_;
	std::vector<shared_ptr<ziMeshingChunk> > workers;

	FOR_EACH(it, levelZeroChunks_) {

		double error = OmMeshParams::GetInitialError();
		OmMipChunkCoord c = *it;
		shared_ptr<ziMeshingChunk> worker(new ziMeshingChunk(segmentationId_, c,
															 mipMeshManager_,
															 this));
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

	zi::task_manager::simple manager(getQueueSize());

	manager.start();

	FOR_EACH(it, workers)
		manager.push_back(*it);

	manager.join();
}

int ziMesher::getQueueSize() {
	//TODO: retrieve from omLocalPreferences? (purcaro)
	//Update by aleks: this actually shouldn't be too big, since
	// each thread eats a lot of memory (pre loads the data)
	int idealNum = zi::system::memory_gb / 3;
	if( idealNum < 2){
		idealNum = 2;
	}
	printf("ziMesher: will use %d threads\n", idealNum);
	return idealNum;
}
