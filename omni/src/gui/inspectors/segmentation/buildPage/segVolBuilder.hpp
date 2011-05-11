#pragma once

#include "volume/build/omBuildSegmentation.hpp"

namespace om {
namespace segmentationInspector {

class segVolBuilder {
public:
    void Build(const SegmentationDataWrapper& sdw,
               const QString& whatOrHowToBuild,
               const QFileInfoList& fileNamesAndPaths)
    {
        OmBuildSegmentation bs(sdw);

        bs.setFileNamesAndPaths(fileNamesAndPaths);

        if ("Data" == whatOrHowToBuild){
            bs.BuildImage();

        } else if ("Mesh" == whatOrHowToBuild){
            bs.BuildMesh();

        } else if ("Data & Mesh" == whatOrHowToBuild){
            bs.BuildAndMeshSegmentation();

        } else if ("Load Dendrogram" == whatOrHowToBuild){
            bs.loadDendrogram();

        } else if( "Blank Volume" == whatOrHowToBuild){
            bs.buildBlankVolume();

        } else {
            throw OmArgException("unknown option", whatOrHowToBuild);
        }
    }
};

} // namespace segmentationInspector
} // namespace om
