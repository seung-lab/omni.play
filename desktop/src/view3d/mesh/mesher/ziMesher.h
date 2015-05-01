#pragma once
#include "precomp.h"

#include "chunks/omChunkUtils.hpp"
#include "common/common.h"
#include "view3d/mesh/mesher/MeshCollector.hpp"
#include "view3d/mesh/mesher/TriStripCollector.hpp"
#include "view3d/mesh/io/omMeshMetadata.hpp"
#include "view3d/mesh/omMeshParams.hpp"
#include "utility/omLockedPODs.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omSegmentation.h"
#include "view3d/mesh/mesher/omMesherProgress.hpp"


class ziMesher {

public:
  ziMesher(OmSegmentation* segmentation, const double threshold);
  ~ziMesher();

 void MeshFullVolume();
 void Progress(std::shared_ptr<om::gui::progress> p); 
 std::shared_ptr<om::gui::progress> Progress();

private:
  void init();
  void addValuesFromChunkAndDownsampledChunks(const om::coords::Chunk& mip0coord);
  void downsampleSegThroughAllMipLevels(const om::coords::Chunk& mip0coord, const om::chunk::UniqueValues& segIDsMip0);
  void registerSegIDs(const om::coords::Chunk& mip0coord, const om::coords::Chunk& c, const om::chunk::UniqueValues& segIDs); 
  void processSingleSegment( int id, double scale, zi::vl::vec3d trans, zi::shared_ptr<zi::mesh::simplifier<double> > simplifier, std::vector<MeshCollector*>* targets);
  void setupMarchingCube(zi::mesh::marching_cubes<int>& cube_marcher, const om::coords::Chunk& c);
  void processChunk(const om::coords::Chunk& coord);
  static int numberParallelChunks();  


  OmSegmentation* const segmentation_;
  const int rootMipLevel_;
  const double threshold_;

  std::map<om::coords::Chunk, std::vector<MeshCollector*> > occurances_;
  std::map<om::coords::Chunk, MeshCollector*> chunkCollectors_;

  OmMeshManager* const meshManager_;
  std::shared_ptr<OmMeshWriterV2> meshWriter_;

  const int numParallelChunks_;
  const int numThreadsPerChunk_;
  const double downScallingFactor_;

  om::mesher::progress progress_; 

};
