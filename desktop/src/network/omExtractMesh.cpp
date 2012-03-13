#include "network/omExtractMesh.h"
#include "utility/segmentationDataWrapper.hpp"
#include "mesh/omMeshManagers.hpp"

std::string OmExtractMesh::WriteOutMesh(const OmID segmentationID, const OmSegID segID,
                        const int mipLevel, const int x, const int y, const int z)
{
    SegmentationDataWrapper sdw(segmentationID);
    if(!sdw.IsValidWrapper()){
        return "invalid segmentation";
    }

    const OmChunkCoord coord(mipLevel, x, y, z);
    OmSegmentation& vol = sdw.GetSegmentation();

    if(!vol.Coords().ContainsMipChunkCoord(coord)){
        return "invalid coord";
    }

    OmMeshPtr mesh;
    vol.MeshManagers()->GetMesh(mesh, coord, segID, 1, om::BLOCKING);

    if(!mesh){
        return "no mesh";
    }

    OmDataForMeshLoad* data = mesh->Data();

    if(data->TrianDataCount()){
        return "old meshes not supported";
    }

    if(!data->HasData()){
        return "no data";
    }

    const OmUUID uuid;

    const QString fnBase("/var/www/temp_omni_imgs/segmentation-1-meshes/" +
                         QString::fromStdString(uuid.Str()) + ".");

    writeFile(fnBase + "vertexIndexData", data->VertexIndex(), data->VertexIndexNumBytes());
    writeFile(fnBase + "vertexData", data->VertexData(), data->VertexDataNumBytes());
    writeFile(fnBase + "stripData", data->StripData(), data->StripDataNumBytes());

    return uuid.Str();
}
