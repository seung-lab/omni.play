#include <limits>
#include <zi/utility/assert.hpp>

#include "datalayer/memMappedFile.hpp"

#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "utility/timer.hpp"
#include "utility/UUID.hpp"

#include "mesh/meshReader.hpp"
#include "mesh/mesher/ziMesher.hpp"

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
                                       coords::chunk chunkCoord, 
                                       int32_t segId)
{
    boost::shared_ptr<mesh::data> data;

    try
    {
        mesh::reader reader(uri, chunkCoord);
        mesh::dataEntry* de = reader.GetDataEntry(segId);
        if(!de || !de->wasMeshed) {
            return boost::shared_ptr<mesh::data>();
        }

        int numVertices = de->vertexData.numElements;
        if (numVertices < numeric_limits<uint16_t>::max())
        {
            data = reader.Read(segId);
        } else {
            std::cout << "Mesh had " << numVertices << " Vertices." << std::endl;
        }
    }
    catch (exception e)
    {
        return boost::shared_ptr<mesh::data>();
    }

    if(!data.get()) {
        return boost::shared_ptr<mesh::data>();
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
    ZI_ASSERT( points_length%6==0 );

    for ( std::size_t i = 0; i < points_length; i += 6 )
    {
        ss << "v " << points[i] << ' ' << points[i+1] << ' ' << points[i+2] << '\n';
    }

    for ( std::size_t i = 0; i < points_length; i += 6 )
    {
        ss << "vn " << points[i+3] << ' ' << points[i+4] << ' ' << points[i+5] << '\n';
    }

    for ( std::size_t i = 0; i < strips_length; i += 2 )
    {
        if(strips[i] == strips[i+1]) {
            continue;
        }

        bool even = true;
        for ( uint32_t j = strips[i]; j < strips[i] + strips[i+1] - 2; ++j )
        {
            if ( even )
            {
                ss << "f " << indices[j]+1 << "//" << indices[j]+1 << ' '
                           << indices[j+1]+1 << "//" << indices[j+1]+1 << ' '
                           << indices[j+2]+1 << "//" << indices[j+2]+1 << '\n';
            }
            else
            {
                ss << "f " << indices[j+2]+1 << "//" << indices[j+2]+1 << ' '
			   << indices[j+1]+1 << "//" << indices[j+1]+1 << ' '
			   << indices[j]+1 << "//" << indices[j]+1 << '\n';
            }
            even = !even;
        }
    }
    return ss.str();
}

void get_mesh(std::string& _return,
              const std::string& uri,
              const server::vector3i& chunk,
              int32_t mipLevel,
              int32_t segId)
{
    boost::shared_ptr<mesh::data> data = 
        get_data(uri, coords::chunk(mipLevel, chunk.x, chunk.y, chunk.z), segId);

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

void get_remesh(std::string& _return,
                const std::string& uri,
                const server::vector3i& chunk,
                int32_t mipLevel,
                int32_t segId)
{

}

void get_obj(std::string& _return,
             const std::string& uri,
             const server::vector3i& chunk,
             int32_t mipLevel,
             int32_t segId)
{
    _return.clear();
    std::cout << "Getting some Obj!!!" << std::endl;
    mesh::reader reader(uri, coords::chunk(mipLevel, chunk.x, chunk.y, chunk.z));
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
                               data->VertexDataCount()*6,
                               data->VertexIndex(),
                               data->VertexIndexCount(),
                               data->StripData(),
                               data->StripDataCount()*2);
}

}
}
