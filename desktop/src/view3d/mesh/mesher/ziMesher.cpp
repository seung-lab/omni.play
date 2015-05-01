#include "ziMesher.h"


ziMesher::ziMesher(OmSegmentation* segmentation, const double threshold)
  : segmentation_(segmentation),
    rootMipLevel_(segmentation->Coords().RootMipLevel()),
    threshold_(threshold),
    chunkCollectors_(),
    meshManager_(segmentation->MeshManager(threshold)),
    meshWriter_(new OmMeshWriterV2(meshManager_)),
    numParallelChunks_(numberParallelChunks()),
    numThreadsPerChunk_(zi::system::cpu_count / 2),
    downScallingFactor_(OmMeshParams::GetDownScallingFactor()) {
  log_infos << "ziMesher: will process " << numParallelChunks_
            << " chunks at a time";
  log_infos << "ziMesher: will use " << numThreadsPerChunk_
            << " threads per chunk";
}

ziMesher::~ziMesher() {
  FOR_EACH(iter, chunkCollectors_) { delete iter->second; }
}

void ziMesher::MeshFullVolume() {
  init();
  meshWriter_->Join();
  meshWriter_->CheckEverythingWasMeshed();
  meshManager_->Metadata()->SetMeshedAndStorageAsChunkFiles();
}

std::shared_ptr<om::gui::progress> ziMesher::Progress() { return progress_.Progress(); }
void ziMesher::Progress(std::shared_ptr<om::gui::progress> p) { progress_.Progress(p); }



void ziMesher::init() {
  auto levelZeroChunks = segmentation_->Coords().MipChunkCoords(0);
  progress_.SetTotalNumChunks(levelZeroChunks->size());

  for (auto& chunk : *levelZeroChunks) {
    addValuesFromChunkAndDownsampledChunks(chunk);
  }

  log_infos << "starting meshing...";

  zi::task_manager::simple manager(numParallelChunks_);
  manager.start();

  for (auto& chunk : *levelZeroChunks) {
    manager.push_back(
          zi::run_fn(zi::bind(&ziMesher::processChunk, this, chunk)));
  }


  manager.join();

  log_infos << "done meshing...";
}

void ziMesher::addValuesFromChunkAndDownsampledChunks(const om::coords::Chunk& mip0coord) {

  const auto segIDs = segmentation_->UniqueValuesDS().Get(mip0coord);

  if (!segIDs) {
    log_debugs << "Cannot load UniqueValues";
    return;
  }

  chunkCollectors_.insert(std::make_pair(
                            mip0coord, new MeshCollector(mip0coord, *meshWriter_.get())));

  occurances_[mip0coord].push_back(chunkCollectors_[mip0coord]);

  for (auto cid : *segIDs) {
    chunkCollectors_[mip0coord]->registerMeshPart(cid);
  }
  downsampleSegThroughAllMipLevels(mip0coord, *segIDs);
}

void ziMesher::downsampleSegThroughAllMipLevels(
    const om::coords::Chunk& mip0coord,
    const om::chunk::UniqueValues& segIDsMip0) {
  om::coords::Chunk c = mip0coord.ParentCoord();

  // corner case: no MIP levels >0
  while (c.mipLevel() <= rootMipLevel_) {
    registerSegIDs(mip0coord, c, segIDsMip0);

    c = c.ParentCoord();
  }
}

void ziMesher::registerSegIDs(const om::coords::Chunk& mip0coord,
                              const om::coords::Chunk& c, const om::chunk::UniqueValues& segIDs) {
  if (chunkCollectors_.count(c) == 0) {
    chunkCollectors_.insert(
          std::make_pair(c, new MeshCollector(c, *meshWriter_.get())));
  }

  for (auto cid : segIDs) {
    chunkCollectors_[c]->registerMeshPart(cid);
  }

  occurances_[mip0coord].push_back(chunkCollectors_[c]);
}

void ziMesher::processSingleSegment(
    int id, double scale, zi::vl::vec3d trans,
    zi::shared_ptr<zi::mesh::simplifier<double> > simplifier,
    std::vector<MeshCollector*>* targets) {
  double max_err = 90.0;

  simplifier->prepare();

  for (auto it : *targets) {
    simplifier->optimize(simplifier->face_count() / downScallingFactor_,
                         max_err);

    std::vector<zi::vl::vec3d> pts;
    std::vector<zi::vl::vec3d> nrms;
    std::vector<uint32_t> vtices;
    std::vector<uint32_t> starts;
    std::vector<uint32_t> lengths;

    simplifier->stripify(pts, nrms, vtices, starts, lengths);

    TriStripCollector* tsc = it->getMesh(id);

    if (tsc &&
        tsc->append(pts, nrms, vtices, starts, lengths, scale, trans) == 0) {
      it->save(id);
    }

    max_err *= 16;
  }

  simplifier.reset();
}

void ziMesher::setupMarchingCube(zi::mesh::marching_cubes<int>& cube_marcher,
                                 const om::coords::Chunk& c) {
  OmImage<uint32_t, 3> chunkData = OmChunkUtils::GetMeshOmImageData(segmentation_, c);

  OmChunkUtils::RewriteChunkAtThreshold(segmentation_, chunkData, threshold_);

  const auto* chunkDataRaw = static_cast<const om::common::SegID*>(chunkData.getScalarPtr());

  cube_marcher.marche(reinterpret_cast<const int*>(chunkDataRaw), 129, 129, 129);
}

void ziMesher::processChunk(const om::coords::Chunk& coord) {
  static const int chunkDim = segmentation_->Coords().ChunkDimensions().x;

  const om::coords::DataBbox  dst = coord.BoundingBox(segmentation_->Coords());
  const om::coords::NormBbox& dstBbox = dst.ToNormBboxExclusive();

  Vector3f minPos = dstBbox.getMin();
  const zi::vl::vec3d translate(minPos.z, minPos.y, minPos.x);


  Vector3f dstDim = dstBbox.getDimensions();
  zi::vl::vec3d scale(dstDim.x / chunkDim,
                      dstDim.y / chunkDim,
                      dstDim.z / chunkDim);

  const double maxScale = scale.max();
  scale /= maxScale;
  scale *= 0.5;

  const auto segIDs = segmentation_->UniqueValuesDS().Get(coord);

  if (segIDs->size() > 0) {
    zi::mesh::marching_cubes<int> cube_marcher;
    setupMarchingCube(cube_marcher, coord);

    zi::task_manager::simple manager(numThreadsPerChunk_);
    manager.start();

    FOR_EACH(it, cube_marcher.meshes()) {
      const om::common::SegID segID = it->first;

      if (segIDs->contains(segID)) {
        auto spfy = std::make_shared<zi::mesh::simplifier<double>>(0);

        cube_marcher.fill_simplifier<double>(
              *spfy, segID, 0, 0, 0, scale.at(2), scale.at(1), scale.at(0));

        manager.push_back(zi::run_fn(
                            zi::bind(&ziMesher::processSingleSegment, this, segID, maxScale,
                                     translate, spfy, &occurances_[coord])));
      }
    }

    cube_marcher.clear();

    manager.join();
  } else {
    log_infos << "Skipping Chunk " << coord
              << " b/c there's nothing in there";
  }

  progress_.ChunkCompleted(coord);
}

int ziMesher::numberParallelChunks() {
  // each thread eats a lot of memory (pre loads the data)
  const int megsMemNeededPerChunk = 5000;

  const int sysMemMegs = zi::system::memory::total_mb();

  const int numChunks = sysMemMegs / megsMemNeededPerChunk;

  if (numChunks < 2) {
    return 2;
  }

  return numChunks;
}

