#pragma once
#include "precomp.h"

#include "chunks/omChunkUtils.hpp"
#include "common/common.h"
#include "mesh/mesher/MeshCollector.hpp"
#include "mesh/mesher/TriStripCollector.hpp"
#include "mesh/omMeshParams.hpp"
#include "utility/lockedPODs.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"
#include "mesh/mesher/omMesherProgress.hpp"

class ziMesher {
 public:
  ziMesher(OmSegmentation& segmentation)
      : segmentation_(segmentation),
        rootMipLevel_(segmentation.Coords().RootMipLevel()),
        chunkCollectors_(),
        meshManager_(segmentation.MeshManager()),
        meshWriter_(new OmMeshWriterV2(meshManager_)),
        numParallelChunks_(numberParallelChunks()),
        numThreadsPerChunk_(zi::system::cpu_count / 2),
        downScallingFactor_(OmMeshParams::GetDownScallingFactor()) {
    log_debug(unknown, "ziMesher: will process %d chunks at a time",
              numParallelChunks_);
    log_debug(unknown, "ziMesher: will use %d threads per chunk",
              numThreadsPerChunk_);
  }

  ~ziMesher() {
    FOR_EACH(iter, chunkCollectors_) { delete iter->second; }
  }

  void MeshFullVolume() {
    init();
    meshWriter_->Join();
    meshWriter_->CheckEverythingWasMeshed();
    meshManager_.Metadata().SetMeshedAndStorageAsChunkFiles();
  }

  // void RemeshFullVolume()
  // {
  // if(redownsample){
  //     segmentation_.VolData()->downsample(segmentation_);
  // }
  //     OmChunkUtils::RefindUniqueChunkValues(segmentation_.id());
  // }

  std::shared_ptr<om::gui::progress> Progress() { return progress_.Progress(); }

  void Progress(std::shared_ptr<om::gui::progress> p) { progress_.Progress(p); }

 private:
  OmSegmentation& segmentation_;
  const int rootMipLevel_;

  std::map<om::coords::Chunk, std::vector<MeshCollector*>> occurances_;
  std::map<om::coords::Chunk, MeshCollector*> chunkCollectors_;

  OmMeshManager& meshManager_;
  std::unique_ptr<OmMeshWriterV2> meshWriter_;

  const int numParallelChunks_;
  const int numThreadsPerChunk_;
  const double downScallingFactor_;

  om::mesher::progress progress_;

  void init() {
    std::shared_ptr<std::vector<om::coords::Chunk>> levelZeroChunks =
        segmentation_.Coords().MipChunkCoords(0);

    progress_.SetTotalNumChunks(levelZeroChunks->size());

    for (auto& chunk : *levelZeroChunks) {
      addValuesFromChunkAndDownsampledChunks(chunk);
    }

    log_debugs << "starting meshing...";

    zi::task_manager::simple manager(numParallelChunks_);
    manager.start();

    for (auto& chunk : *levelZeroChunks) {
      manager.push_back(
          zi::run_fn(zi::bind(&ziMesher::processChunk, this, chunk)));
    }

    manager.join();

    log_debugs << "done meshing...";
  }

  void addValuesFromChunkAndDownsampledChunks(
      const om::coords::Chunk& mip0coord) {
    const std::shared_ptr<om::chunk::UniqueValues> segIDs =
        segmentation_.ChunkUniqueValues()->Get(mip0coord);

    if (!segIDs) {
      return;
    }

    chunkCollectors_.insert(std::make_pair(
        mip0coord, new MeshCollector(mip0coord, meshWriter_.get())));

    occurances_[mip0coord].push_back(chunkCollectors_[mip0coord]);

    for (auto cid, *segIDs.get().Values) {
      chunkCollectors_[mip0coord]->registerMeshPart(cid);
    }

    downsampleSegThroughAllMipLevels(mip0coord, segIDs.get());
    // downsampleSegThroughViewableMipLevels(mip0coord, segIDs);
  }

  void downsampleSegThroughAllMipLevels(
      const om::coords::Chunk& mip0coord,
      const om::chunk::UniqueValues& segIDsMip0) {
    om::coords::Chunk c = mip0coord.ParentCoord();

    // corner case: no MIP levels >0
    while (c.mipLevel() <= rootMipLevel_) {
      registerSegIDs(mip0coord, c, *segIDsMip0.Values);

      c = c.ParentCoord();
    }
  }

  void downsampleSegThroughViewableMipLevels(
      const om::coords::Chunk& mip0coord,
      const om::chunk::UniqueValues& segIDsMip0) {
    om::coords::Chunk c = mip0coord.ParentCoord();

    // corner case: no MIP levels >0
    while (c.mipLevel() <= rootMipLevel_) {
      std::deque<om::common::SegID> commonIDs;

      const std::shared_ptr<om::chunk::UniqueValues> segIDs =
          segmentation_.ChunkUniqueValues()->Get(c);

      if (!segIDs) {
        return;
      }

      for (auto cid, *segIDsMip0.Values) {
        if (segIDs.get().contains(cid)) {
          commonIDs.push_back(cid);
        }
      }

      if (commonIDs.empty()) {
        break;
      }

      registerSegIDs(mip0coord, c, commonIDs);

      c = c.ParentCoord();
    }
  }

  template <typename C>
  void registerSegIDs(const om::coords::Chunk& mip0coord,
                      const om::coords::Chunk& c, const C& segIDs) {
    if (chunkCollectors_.count(c) == 0) {
      chunkCollectors_.insert(
          std::make_pair(c, new MeshCollector(c, meshWriter_.get())));
    }

    for (auto cid, segIDs) {
      chunkCollectors_[c]->registerMeshPart(cid);
    }

    occurances_[mip0coord].push_back(chunkCollectors_[c]);
  }

  void processSingleSegment(
      int id, double scale, zi::vl::vec3d trans,
      zi::shared_ptr<zi::mesh::simplifier<double>> simplifier,
      std::vector<MeshCollector*>* targets) {
    double max_err = 90.0;

    simplifier->prepare();

    FOR_EACH(it, *targets) {
      simplifier->optimize(simplifier->face_count() / downScallingFactor_,
                           max_err);

      std::vector<zi::vl::vec3d> pts;
      std::vector<zi::vl::vec3d> nrms;
      std::vector<uint32_t> vtices;
      std::vector<uint32_t> starts;
      std::vector<uint32_t> lengths;

      simplifier->stripify(pts, nrms, vtices, starts, lengths);

      TriStripCollector* tsc = (*it)->getMesh(id);

      if (tsc &&
          tsc->append(pts, nrms, vtices, starts, lengths, scale, trans) == 0) {
        (*it)->save(id);
      }

      max_err *= 16;
    }

    simplifier.reset();
  }

  void setupMarchingCube(zi::mesh::marching_cubes<int>& cube_marcher,
                         const om::coords::Chunk& chunk) {
    OmImage<uint32_t, 3> chunkData =
        OmChunkUtils::GetMeshOmImageData(segmentation_.ChunkDS(), chunk);

    const auto* chunkDataRaw =
        static_cast<const om::common::SegID*>(chunkData.getScalarPtr());

    cube_marcher.marche(reinterpret_cast<const int*>(chunkDataRaw), 129, 129,
                        129);
  }

  void processChunk(const om::coords::Chunk& coord) {
    static const Vector3i chunkDims = segmentation_.Coords().ChunkDimensions();

    const auto& dstBbox = coord.BoundingBox(*segmentation_).ToNormBbox();

    Vector3f dstDim = dstBbox.getDimensions();

    zi::vl::vec3d scale(dstDim.x / chunkDims.x, dstDim.y / chunkDims.y,
                        dstDim.z / chunkDims.z);

    dstDim = dstBbox.getMin();

    const zi::vl::vec3d translate(dstDim.z, dstDim.y, dstDim.x);

    const double maxScale = scale.max();
    scale /= maxScale;
    scale *= 0.5;

    const std::shared_ptr<om::chunk::UniqueValues> segIDs =
        segmentation_.ChunkUniqueValues()->Get(coord);

    if (segIDs && segIDs.get().Values->size() > 0) {
      zi::mesh::marching_cubes<int> cube_marcher;
      setupMarchingCube(cube_marcher, coord);

      zi::task_manager::simple manager(numThreadsPerChunk_);
      manager.start();

      FOR_EACH(it, cube_marcher.meshes()) {
        const auto segID = it->first;

        if (segIDs.get().contains(segID)) {
            auto spfy = std::make_shared<zi::mesh::simplifier<double>>(0));

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
      log_debugs << "Skipping Chunk " << coord
                 << " b/c there's nothing in there";
    }

    progress_.ChunkCompleted(coord);
  }

  static int numberParallelChunks() {
    // each thread eats a lot of memory (pre loads the data)
    const int megsMemNeededPerChunk = 5000;

    const int sysMemMegs = zi::system::memory::total_mb();

    const int numChunks = sysMemMegs / megsMemNeededPerChunk;

    if (numChunks < 2) {
      return 2;
    }

    return numChunks;
  }
};
