
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

    if(file.fail()){
        throw common::ioException("could not write file", fnp);
    }
}

void get_mesh(std::string& _return,
              const std::string& uri,
              const server::vector3i& chunk,
              int32_t segId)
{
    utility::timer t;
    std::string path = str(boost::format("%1%/1.0000/0/%2%/%3%/%4%/")
                           % uri % chunk.x % chunk.y % chunk.z);
    mesh::reader reader(path);

    boost::shared_ptr<mesh::data> data =
        reader.Read(segId, coords::chunk(0, chunk.x, chunk.y, chunk.z));

    if(data->TrianDataCount()){
        throw common::ioException("old meshes not supported");
    }

    if(!data->HasData()){
        throw common::ioException("no data");
    }

    const utility::UUID uuid;

    std::string formatStr = "/var/www/temp_omni_imgs/segmentation-1-meshes/%1%.%2%";

    std::stringstream ss;
    std::string fnp;

    writeFile(str(boost::format(formatStr) % uuid.Str() % "vertexIndexData"),
              data->VertexIndex(), data->VertexIndexNumBytes());
    writeFile(str(boost::format(formatStr) % uuid.Str() % "vertexData"),
              data->VertexData(), data->VertexDataNumBytes());
    writeFile(str(boost::format(formatStr) % uuid.Str() % "stripData"),
              data->StripData(), data->StripDataNumBytes());

    _return = uuid.Str();

    std::cout << "get_mesh done: " << t.s_elapsed() << " seconds" << std::endl;
}

}
}
