#include "thrift/server.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

#include "project/project.h"
#include "project/details/projectVolumes.h"
#include "project/details/channelManager.h"
#include "project/details/segmentationManager.h"
#include "tiles/tile.h"
#include "volume/channel.h"
#include "segment/segmentTypes.h"
#include "mesh/io/meshReader.hpp"
#include "utility/UUID.hpp"
#include "utility/timer.hpp"

#include "pipeline/getTileData.hpp"
#include "pipeline/sliceTile.hpp"
#include "pipeline/jpeg.h"
#include "pipeline/encode.hpp"
#include "pipeline/utility.hpp"
//#include "pipeline/filter.hpp"
#include "pipeline/bitmask.hpp"
#include "pipeline/png.hpp"
#include "pipeline/getSegIds.hpp"

#include "boost/format.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
namespace om {
namespace server {

std::ostream& operator<<(std::ostream& o, const vector3d& v) {
    return o << v.x << ", " << v.y << ", " << v.z;
}

class serverHandler : virtual public serverIf {
public:
    void get_chan_tile(server::tile& _return,
                       const server::metadata& vol,
                       const server::vector3d& point,
                       const server::viewType::type view)
    {
        utility::timer t;
        validateMetadata(vol);

        coords::volumeSystem coordSystem(vol);
        coords::globalCoord coord = point;
        coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

        setTileBounds(_return, dc, view);

        using namespace pipeline;

        data_var encoded = mapData(vol.uri, vol.type) >> sliceTile(common::Convert(view), dc)
                                                      >> jpeg(128,128)
                                                      >> encode();

        data<char> out = boost::get<data<char> >(encoded);
        _return.data = std::string(out.data.get(), out.size);

        std::cout << "get_chan_tile done: " << t.s_elapsed() << " seconds" << std::endl;
    }

    void get_seg_tiles(std::map<std::string, tile> & _return,
                       const metadata& vol,
                       const int32_t segId,
                       const bbox& segBbox,
                       const viewType::type view)
    {
        utility::timer t;

        validateMetadata(vol);

        coords::volumeSystem coordSystem(vol);

        pipeline::mapData dataSrc(vol.uri, vol.type);

        vector3d min = common::twist(segBbox.min, view);
        vector3d max = common::twist(segBbox.max, view);
        vector3i dims = common::twist(vol.chunkDims, view);
        vector3i res = common::twist(vol.resolution, view);

        for(int x = min.x; x <= max.x; x += dims.x * res.x) {
            for(int y = min.y; y <= max.y; y += dims.y * res.y) {
                for(int z = min.z; z <= max.z; z += res.z) // always depth when twisted
                {
                    coords::globalCoord coord = common::twist(coords::globalCoord(x,y,z), view);
                    coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

                    tile t;
                    makeSegTile(t, dataSrc, dc, view, segId);
                    std::stringstream ss;
                    ss << t.bounds.min.x << "-"
                       << t.bounds.min.y << "-"
                       << t.bounds.min.z << "-"
                       << segId;
                    _return[ss.str()] = t;
                }
            }
        }

        std::cout << "get_seg_tiles done: " << t.s_elapsed() << " seconds" << std::endl;
    }

    int32_t get_seg_id(const metadata& vol, const vector3d& point)
    {
        utility::timer t;
        validateMetadata(vol);

        coords::volumeSystem coordSystem(vol);

        coords::globalCoord coord = point;
        coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

        using namespace pipeline;

        mapData dataSrc(vol.uri, vol.type);

        data_var id = dataSrc >> getSegIds(dc);

        std::cout << "get_seg_id done: " << t.s_elapsed() << " seconds" << std::endl;

        return boost::get<data<uint32_t> >(id).data.get()[0];
    }

    void get_seg_ids(std::vector<int32_t> & _return,
                     const metadata& vol,
                     const vector3d& point,
                     const double radius,
                     const viewType::type view)
    {
        utility::timer t;
        validateMetadata(vol);

        coords::volumeSystem coordSystem(vol);

        coords::globalCoord coord = point;
        coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

        using namespace pipeline;

        mapData dataSrc(vol.uri, vol.type);

        data_var ids = dataSrc >> getSegIds(dc, radius, view);

        data<uint32_t> out = boost::get<data<uint32_t> >(ids);

        _return.resize(out.size);
        uint32_t* outData = out.data.get();
        std::copy(outData, &outData[out.size], _return.begin());
        std::cout << "get_seg_id done: " << t.s_elapsed() << " seconds" << std::endl;
    }

    void get_seg_bbox(bbox& _return, const std::string&path, const int32_t segId)
    {
        utility::timer t;
        if (!file::exists(path)) {
            throw common::argException("Cannot find path");
        }

        const uint32_t pageSize = 100000;
        const uint32_t pageNum = segId / pageSize;
        const uint32_t idx = segId % pageSize;
        const std::string fname = str( boost::format("%1%/segment_page%2%.data.ver4")
                                       % path % pageNum);

        datalayer::memMappedFile<segment::data> page(fname);

        const segment::data& d = page.GetPtr()[idx];

        _return.min.x = d.bounds.getMin().x;
        _return.min.y = d.bounds.getMin().y;
        _return.min.z = d.bounds.getMin().z;
        _return.max.x = d.bounds.getMax().x;
        _return.max.y = d.bounds.getMax().y;
        _return.max.z = d.bounds.getMax().z;

        std::cout << "get_seg_bbox done: " << t.s_elapsed() << " seconds" << std::endl;
    }

    double compare_results(const std::vector<result> & old_results, const result& new_result)
    {
        // Your implementation goes here
        printf("compare_results\n");
        return 0;
    }

    void get_mesh(std::string& _return,
                  const std::string& uri,
                  const vector3i& chunk,
                  int32_t segId)
    {
        utility::timer t;
        std::string path = str(boost::format("%1%/1.0000/0/%2%/%3%/%4%/")
                                % uri % chunk.x % chunk.y % chunk.z);
        std::cout << path << std::endl;
        mesh::reader reader(path);

        boost::shared_ptr<mesh::data> data =
            reader.Read(segId, coords::chunkCoord(0, chunk.x, chunk.y, chunk.z));

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

private:
    void validateMetadata(const metadata& meta)
    {
        if(!file::exists(meta.uri)) {
            throw common::argException("Cannot find requested volu2me.");
        }
/*
        int factor = math::pow2int(meta.mipLevel);

        std::size_t size = (meta.bounds.max.x - meta.bounds.min.x) *
            (meta.bounds.max.y - meta.bounds.min.y) *
            (meta.bounds.max.z - meta.bounds.min.z) /
            (factor * factor * factor);

        switch(meta.type)
        {
        case dataType::INT32:
        case dataType::UINT32:
        case dataType::FLOAT:
            size *= 4;
            break;
        }

        if(file::numBytes(meta.uri) != size) {
            throw common::argException("Incorrect file size given bounds, mip level and data type.");
        }
*/

        if(meta.resolution.x <= 0 || meta.resolution.y <= 0 || meta.resolution.z <= 0) {
            throw common::argException("Resolution must be greater than 0.");
        }

        if(meta.chunkDims.x <= 0 || meta.chunkDims.y <= 0 || meta.chunkDims.z <= 0) {
            throw common::argException("Chunk Dims must be greater than 0.");
        }
    }

    void makeSegTile(tile& t,
                     const pipeline::dataSrcs& src,
                     const coords::dataCoord& dc,
                     const viewType::type& view,
                     uint32_t segId) const
    {
        t.view = view;
        setTileBounds(t, dc, view);

        using namespace pipeline;

        data_var encoded = src >> sliceTile(common::Convert(view), dc)
                               >> bitmask(segId)
                               >> png(128,128)
                               >> encode();

        data<char> out = boost::get<data<char> >(encoded);
        t.data = std::string(out.data.get(), out.size);
    }

    inline void setTileBounds(tile& t,
                              const coords::dataCoord dc,
                              const viewType::type& view) const
    {
        coords::chunkCoord cc = dc.toChunkCoord();
        int depth = dc.toTileDepth(common::Convert(view));
        coords::dataBbox bounds = cc.chunkBoundingBox(dc.volume());

        vector3d min = common::twist(bounds.getMin().toGlobalCoord(), view);
        vector3d max = common::twist(bounds.getMax().toGlobalCoord(), view);

        min.z += depth;
        max.z = min.z;

        t.bounds.min = common::twist(min, view);
        t.bounds.max = common::twist(max, view);
    }

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

};

}
}
