#pragma once

#include "volume/build/omBuildSegmentation.hpp"

namespace om {
namespace segmentationInspector {

class segVolBuilder {
 private:
  const SegmentationDataWrapper sdw_;
  const QString whatOrHowToBuild_;
  const QFileInfoList fileNamesAndPaths_;

 public:
  segVolBuilder(const SegmentationDataWrapper& sdw,
                const QString& whatOrHowToBuild,
                const QFileInfoList& fileNamesAndPaths)
      : sdw_(sdw),
        whatOrHowToBuild_(whatOrHowToBuild),
        fileNamesAndPaths_(fileNamesAndPaths) {}

  void Build() {
    OmBuildSegmentation bs(sdw_);

    bs.setFileNamesAndPaths(fileNamesAndPaths_);

    if ("Data" == whatOrHowToBuild_) {
      bs.BuildImage();

    } else if ("Mesh" == whatOrHowToBuild_) {
      bs.BuildMesh();

    } else if ("Data & Mesh" == whatOrHowToBuild_) {
      bs.BuildAndMeshSegmentation();

    } else if ("Load Dendrogram" == whatOrHowToBuild_) {
      bs.LoadDendrogram();

    } else if ("Blank Volume" == whatOrHowToBuild_) {
      bs.BuildBlankVolume();

    } else {
      throw om::ArgException("unknown option");
    }
  }

  void Build(std::shared_ptr<om::gui::progress> p) {
    OmBuildSegmentation bs(sdw_);

    bs.setFileNamesAndPaths(fileNamesAndPaths_);

    if ("Data" == whatOrHowToBuild_) {
      bs.BuildImage();

    } else if ("Mesh" == whatOrHowToBuild_) {
      bs.BuildMesh(p);

    } else if ("Data & Mesh" == whatOrHowToBuild_) {
      bs.BuildAndMeshSegmentation(p);

    } else if ("Load Dendrogram" == whatOrHowToBuild_) {
      bs.LoadDendrogram();

    } else if ("Blank Volume" == whatOrHowToBuild_) {
      bs.BuildBlankVolume();

    } else {
      throw om::ArgException("unknown option");
    }
  }
};

}  // namespace segmentationInspector
}  // namespace om
