#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshCoord.h"
#include "mesh/omMipMeshManager.h"
#include "project/omProject.h"
#include "volume/omSegmentation.h"
#include "zi/base/base.h"
#include "zi/base/time.h"
#include "zi/mesh/HalfEdge.hpp"
#include "zi/mesh/MarchingCubes.hpp"
#include "zi/mesh/QuadraticErrorSimplification.hpp"
#include "zi/mesh/ext/TriStrip/TriStripper.h"
#include "zi/vector/Vec.hpp"
#include "ziMesher.h"
#include "ziMesherManager.h"
#include "ziMeshingChunk.h"

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstdlib>
#include <cmath>

using namespace zi::vector;
using boost::shared_ptr;

#include "common/omDebug.h"

ziMeshingChunk::ziMeshingChunk(int segId, OmMipChunkCoord c,
                               OmMipMeshManager *mmm, ziMesher* mesher) :
	segmentationId_(segId),
	mipCoord_(c),
	mipMeshManager_(mmm),
	mesher_(mesher)
{}

void ziMeshingChunk::run()
{
	const zi::thread_id_type threadId = zi::this_thread::id();
	ziTimer ztimer;
	ztimer.start();

	loadChunk_();

	Vector3<int> dims(129, 129, 129);
	OmImage<uint32_t, 3> image = chunk_->GetMeshOmImageData();
	const OmSegID *pScalarData = static_cast<const OmSegID*>(image.getScalarPtr());

	std::cout << "thread " << std::hex << threadId << std::dec;
	std::cout << ": chunk " << mipCoord_ << " data loaded\n";

	float maxScale = std::max(std::max(scale_.x(), scale_.y()), scale_.z());
	zi::vector::Vec3d scale = scale_ / maxScale;
	scale_ = zi::vector::Vec3d(maxScale, maxScale, maxScale);

	boost::shared_ptr<boost::unordered_map<int, QuickMesh> > qMeshes =
		zi::QuickMarchingCubes(dims.z-1, dims.y-1, dims.x-1, (int*)pScalarData);

	std::cout << "\t" << "thread " << std::hex << threadId << std::dec;
	std::cout << ": chunk " << mipCoord_ << ": Marching Cubes Done "
			  << "(" << ztimer.dTotal() - ztimer.dLap() << " secs)" << "\n";

	double scaleZ = scale.z() * 0.01;
	double scaleY = scale.y() * 0.01;
	double scaleX = scale.x() * 0.01;

	std::map<int, zi::Mesh<Vec3d> > meshes;

	FOR_EACH (it, *qMeshes) {
		meshes[it->first].fromQuickMesh(it->second, scaleZ, scaleY, scaleX);
		meshes[it->first].clearMaps();
		meshes[it->first].calculateVertexNormals();
	}
	qMeshes->clear();

	std::cout << "\t" << "thread " << std::hex << threadId << std::dec;
	std::cout << ": chunk " << mipCoord_ << ": HalfEdge Meshes Created "
			  << "(" << ztimer.dTotal() - ztimer.dLap() << " secs)" << "\n";


	std::map<int, shared_ptr<zi::QuadraticErrorSimplifier> > simplifiers;
	FOR_EACH (it, meshes) {
		simplifiers[it->first] =
			shared_ptr<zi::QuadraticErrorSimplifier>
			(new zi::QuadraticErrorSimplifier(it->second));
	}

	std::cout << "\t" << "thread " << std::hex << threadId << std::dec;
	std::cout << ": chunk " << mipCoord_ << ": QEM Simplifiers Created "
			  << "(" << ztimer.dTotal() - ztimer.dLap() << " secs)" << "\n";


	int idx = 0;
	FOR_EACH (it, toDeliver_) {

		std::map<int, std::pair<Tri::tri_stripper::primitives_vector,
			std::vector< zi::Vertex< Vec3d > *> > > stripified;

		FOR_EACH (sit, simplifiers) {
			sit->second->runQueue((idx == 0) ?
								  (meshes[sit->first].faces_.size() / 2) :
								  (meshes[sit->first].faces_.size() / 2),
								  it->second, 0);
			meshes[sit->first].toStrips(&stripified[sit->first].first,
										stripified[sit->first].second);
		}

		idx++;
		std::cout << "\t" << "thread " << std::hex << threadId << std::dec;
		std::cout << ": chunk " << mipCoord_ << ": Simplification Round # " << idx << " done "
				  << "(" << ztimer.dTotal() - ztimer.dLap() << " secs)" << "\n";

		it->first->receive(stripified, scale_, translate_);

		std::cout << "\t" << "thread " << std::hex << threadId << std::dec;
		std::cout << ": chunk " << mipCoord_ << ": Delivered at " << it->second << " "
				  << "(" << ztimer.dTotal() - ztimer.dLap() << " secs)" << "\n";

		if (it->first->isDone()) {

			std::cout << "\t" << "thread " << std::hex << threadId << std::dec;
			std::cout << ": chunk " << mipCoord_ << ": This Guy is done "
					  << "(" << ztimer.dTotal() - ztimer.dLap() << " secs)" << "\n";

			shared_ptr<std::map<int, shared_ptr<StrippedMesh> > > sMeshes =
				it->first->getStrippedMeshes();

			//sMeshes
			FOR_EACH (mit, *sMeshes) {
				OmMipMeshCoord mmCoor(it->first->getCoor(), mit->first);
				OmMipMeshPtr oMesh = mipMeshManager_->AllocMesh(mmCoor);
				oMesh->setSegmentationID(segmentationId_);

				oMesh->mVertexIndexCount = mit->second->totalIndices_;
				oMesh->mpVertexIndexDataWrap =
					OmDataWrapper<GLuint>::produce(new GLuint[oMesh->mVertexIndexCount],
												   NEW_ARRAY);
				std::copy(mit->second->indices_.begin(),
						  mit->second->indices_.end(),
						  oMesh->mpVertexIndexDataWrap->getPtr<GLuint>());

				oMesh->mVertexCount = mit->second->totalVertices_;
				oMesh->mpVertexDataWrap =
					OmDataWrapper<float>::produce(new GLfloat[oMesh->mVertexCount * 6],
												  NEW_ARRAY);
				std::copy(mit->second->vertices_.begin(),
						  mit->second->vertices_.end(),
						  oMesh->mpVertexDataWrap->getPtr<float>());

				oMesh->mStripCount = mit->second->totalStrips_;
				oMesh->mpStripOffsetSizeDataWrap =
					OmDataWrapper<uint32_t>::produce(new uint32_t[2*oMesh->mStripCount],
													 NEW_ARRAY);
				std::copy(mit->second->strips_.begin(),
						  mit->second->strips_.end(),
						  oMesh->mpStripOffsetSizeDataWrap->getPtr<unsigned int>());

				oMesh->mTrianCount = mit->second->totalTrians_;
				oMesh->mpTrianOffsetSizeDataWrap =
					OmDataWrapper<uint32_t>::produce(new uint32_t[2*oMesh->mTrianCount],
													 NEW_ARRAY);
				std::copy(mit->second->trians_.begin(),
						  mit->second->trians_.end(),
						  oMesh->mpTrianOffsetSizeDataWrap->getPtr<unsigned int>());

				oMesh->Save();
			}

			std::cout << "\t" << "thread " << std::hex << threadId << std::dec;
			std::cout << ": chunk " << mipCoord_ << ": Meshes Saved "
					  << "(" << ztimer.dTotal() - ztimer.dLap() << " secs)"
					  << " TOTAL OF : " << meshes.size() << "\n";

		}
	}
	meshes.clear();

	mesher_->numOfChunksToProcess.sub(1);

	std::cout << "thread " << std::hex << threadId << std::dec;
	std::cout << ": chunk " << mipCoord_ << " finished; "
			  << mesher_->numOfChunksToProcess.get() << " chunks left\n";
}

void ziMeshingChunk::loadChunk_()
{
	//debug ("ziMeshingChunk", "loadChunk_()\n");
	OmProject::GetSegmentation(segmentationId_).GetChunk(chunk_, mipCoord_);
	srcBbox_  = chunk_->GetExtent();
	dstBbox_  = chunk_->GetNormExtent();

	Vector3f srcDim = srcBbox_.getMax() - srcBbox_.getMin();
	srcDim += Vector3<float>::ONE;
	Vector3<float> dstDim = dstBbox_.getMax() - dstBbox_.getMin();
	scale_ = zi::vector::Vec3d(dstDim.x / srcDim.x,
							   dstDim.y / srcDim.y,
							   dstDim.z / srcDim.z);

	dstDim = dstBbox_.getMin();
	translate_ = zi::vector::Vec3d(dstDim.z, dstDim.y, dstDim.x);

	//  cout << "This Chunk has scale: " << scale_
	//       << ", and translate: " << translate_ << "\n";
	//  cout << "orig dim: " << srcDim.x << ", " << srcDim.y
	//       << ", " << srcDim.z << "\n";
}

void ziMeshingChunk::deliverTo(boost::shared_ptr<GrowingMeshes> level,
                               double maxError)
{
	toDeliver_.push_back(std::make_pair(level, maxError));
}
