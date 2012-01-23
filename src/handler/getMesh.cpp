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

boost::shared_ptr<mesh::data> get_data(const std::string& uri,
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
            if(!de || !de->wasMeshed) {
                return boost::shared_ptr<mesh::data>();;
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
            return boost::shared_ptr<mesh::data>();;
        }
    }

    if(data->TrianDataCount()){
        return boost::shared_ptr<mesh::data>();
    }

    if(!data->HasData()){
        return boost::shared_ptr<mesh::data>();
    }
    
    return data;
}

std::string tri_strip_to_obj( const float* points,
                              const std::size_t points_length,
                              const uint32_t* indices,
                              const std::size_t indices_length,
                              const uint32_t* strips,
                              const std::size_t strips_length )
{
    std::stringstream ss;
    for ( std::size_t i = 0; i < points_length; i += 6 )
    {
        ss << "v " << points[i] << ' ' << points[i+1] << ' ' << points[i+2] << '\n';
    }

    for ( std::size_t i = 3; i < points_length; i += 6 )
    {
        ss << "vn " << points[i] << ' ' << points[i+1] << ' ' << points[i+2] << '\n';
    }

    for ( std::size_t i = 0; i < strips_length; i += 2 )
    {
        if(strips[i] == strips[i+1]) {
            continue;
        }
        
        bool even = true;
        for ( uint32_t j = strips[i]; j < strips[i+1] - 2; ++j )
        {
            if ( even )
            {
                ss << "f " << indices[j] << ' ' << indices[j+1] << ' ' << indices[j+2] << '\n';
            }
            else
            {
                ss << "f " << indices[j+2] << ' ' << indices[j+1] << ' ' << indices[j] << '\n';
            }
            even = !even;
        }
    }
    return ss.str();
}

void get_mesh(std::string& _return,
              const std::string& uri,
              const server::vector3i& chunk,
              int32_t segId)
{
    boost::shared_ptr<mesh::data> data = get_data(uri, chunk, segId);

    if(!data.get()) {
        _return = "";
        return;
    }

    const utility::UUID uuid;

    std::string formatStr = "/var/www/temp_omni_imgs/segmentation-1-meshes/%1%.%2%";

    std::string fnp;

    // Move the vertex data to a uint16 array for the sake of webgl.
    boost::shared_ptr<uint16_t> vertexIndexData =
        utility::smartPtr<uint16_t>::MallocNumElements(data->VertexIndexCount());

    std::copy(data->VertexIndex(),
              &data->VertexIndex()[data->VertexIndexCount()],
              vertexIndexData.get());

    writeFile(str(boost::format(formatStr) % uuid.Str() % "vertexIndexData"),
              vertexIndexData.get(), data->VertexIndexNumBytes());
    writeFile(str(boost::format(formatStr) % uuid.Str() % "vertexData"),
              data->VertexData(), data->VertexDataNumBytes());
    writeFile(str(boost::format(formatStr) % uuid.Str() % "stripData"),
              data->StripData(), data->StripDataNumBytes());

    _return = uuid.Str();
}

void get_obj(std::string& _return,
             const std::string& uri,
             const server::vector3i& chunk,
             int32_t segId)
{
    std::cout << "Getting some Obj!!!" << std::endl;
    mesh::reader reader(uri, coords::chunk(0, chunk.x, chunk.y, chunk.z));
    mesh::dataEntry* de = reader.GetDataEntry(segId);
    if(!de || !de->wasMeshed) {
        return;
    }

    boost::shared_ptr<mesh::data> data = reader.Read(segId);

    if(data->TrianDataCount()){
        return;
    }

    if(!data->HasData()){
        return;
    }

    _return = tri_strip_to_obj(data->VertexData(),
                               data->VertexDataCount(),
                               data->VertexIndex(),
                               data->VertexIndexCount(),
                               data->StripData(),
                               data->StripDataCount());
}

}
}
