#include <limits>

#include "datalayer/memMappedFile.hpp"

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

void get_mesh(std::string& _return,
              const std::string& uri,
              const server::vector3i& chunk,
              int32_t segId)
{
    boost::shared_ptr<mesh::data> data;
    // Refactor uri into volume so that we know how many mip levels there are
    for(int mip = 0;; mip++)
    {
        coords::chunk chunkCoord(mip, chunk.x, chunk.y, chunk.z);

        try
        {
            mesh::reader reader(uri, chunkCoord);
            mesh::dataEntry* de = reader.GetDataEntry(segId);
            if(!de || !de->wasMeshed)
            {
                _return = "";
                return;
            }

            int numVertices = de->vertexData.numElements;
            if (numVertices < numeric_limits<uint16_t>::max())
            {
                data = reader.Read(segId);
                break;
            }
        }
        catch (exception e)
        {
            _return = "";
            return;
        }
    }

    if(data->TrianDataCount()){
        _return = "";
        return;
    }

    if(!data->HasData()){
        _return = "";
        return;
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

    // Remove trailing 0s from Strip data.

    writeFile(str(boost::format(formatStr) % uuid.Str() % "vertexIndexData"),
              vertexIndexData.get(), data->VertexIndexNumBytes());
    writeFile(str(boost::format(formatStr) % uuid.Str() % "vertexData"),
              data->VertexData(), data->VertexDataNumBytes());
    writeFile(str(boost::format(formatStr) % uuid.Str() % "stripData"),
              data->StripData(), data->StripDataNumBytes());

    _return = uuid.Str();
}

}
}
