#include <limits>

#include "datalayer/memMappedFile.hpp"
#include "boost/format.hpp"

#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "utility/timer.hpp"
#include "utility/UUID.hpp"

#include "mesh/meshReader.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

template <typename T>
void writeFile(const std::string& fnp, T const*const data, const uint64_t numBytes)
{
    using namespace std;
    std::ofstream file(fnp.c_str(), std::ios::out | std::ios::binary);

    file.write(reinterpret_cast<const char*>(data), numBytes);

//    Appears to be failing even though the file gets written...
//    if(file.fail()){
//        throw ioException("could not write file", fnp);
//    }
}

boost::shared_ptr<mesh::data> get_mesh_data(std::string uri,
                                            coords::chunk chunk,
                                            int32_t segId)
{
    std::string path = str(boost::format("%1%/1.0000/%2%/%3%/%4%/%5%/")
                           % uri % chunk.getLevel() % chunk.X() % chunk.Y() % chunk.Z());
    mesh::reader reader(path);

    boost::shared_ptr<mesh::data> data;
    try {
        data = reader.Read(segId, chunk);
    } catch (ioException e) {
        return boost::shared_ptr<mesh::data>();
    }

    if(data->TrianDataCount()){
        throw ioException("old meshes not supported");
    }

    if(!data->HasData()){
        throw ioException("no data");
    }

    return data;
}

void get_mesh(std::string& _return,
              const std::string& uri,
              const server::vector3i& chunk,
              int32_t segId)
{
    boost::shared_ptr<mesh::data> data;
    for(int mip = 0;; mip++)
    {
        coords::chunk chunkCoord(mip, chunk.x, chunk.y, chunk.z);
        data = get_mesh_data(uri, chunkCoord, segId);

        if(!data) {
            _return = "";
            return;
        }

        if(data->VertexDataCount() < numeric_limits<uint16_t>::max()) {
            break;
        }
    }

    const utility::UUID uuid;

    std::string formatStr = "/var/www/temp_omni_imgs/segmentation-1-meshes/%1%.%2%";

    std::stringstream ss;
    std::string fnp;

    // Move the vertex data to a uint16 array for the sake of webgl.
    boost::shared_ptr<uint16_t> vertexIndexData =
        utility::smartPtr<uint16_t>::MallocNumElements(data->VertexIndexCount());

    std::copy(data->VertexIndex(),
              &data->VertexIndex()[data->VertexIndexCount()],
              vertexIndexData.get());

    // Find last real item in stripData.
    int lastStrip = 0;
    for(int i = 0; i < data->StripDataCount(); i++) {
        if(data->StripData()[i] > 0) {
            lastStrip = i;
        }
    }

    // Remove trailing 0s from Strip data.

    writeFile(str(boost::format(formatStr) % uuid.Str() % "vertexIndexData"),
              vertexIndexData.get(), data->VertexIndexNumBytes());
    writeFile(str(boost::format(formatStr) % uuid.Str() % "vertexData"),
              data->VertexData(), data->VertexDataNumBytes());
    writeFile(str(boost::format(formatStr) % uuid.Str() % "stripData"),
              data->StripData(), lastStrip * sizeof(uint32_t));

    _return = uuid.Str();
}

}
}
