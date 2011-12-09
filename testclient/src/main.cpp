#include "thrift/server.h"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <vector>
#include <iostream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace om::server;
using namespace std;

//#include "wholeVolume.hpp"
#include "throws.hpp"

#include "boost/format.hpp"

#include <cstdlib>
#include <time.h>

ostream& operator<<(ostream& s, vector3d v) {
    return s << v.x << "," << v.y << "," << v.z;
}

ostream& operator<<(ostream& s, bbox b) {
    return s << "{ min = " << b.min << "; max = " << b.max << " }";
}

int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    serverClient client(protocol);
    transport->open();

    std::string root = std::string("/omniData/e2198/e2198_a_s8_101_46_e16_116_61.omni.files/");

    std::map<std::string, tile> tiles;

    metadata meta;
    meta.uri = root;
    meta.bounds.min.x = 0;
    meta.bounds.min.y = 0;
    meta.bounds.min.z = 0;
    meta.bounds.max.x = 1151;
    meta.bounds.max.y = 2047;
    meta.bounds.max.z = 2047;
    meta.resolution.x = 1;
    meta.resolution.y = 1;
    meta.resolution.z = 1;
    meta.type = dataType::UINT32;
    meta.chunkDims.x = 128;
    meta.chunkDims.y = 128;
    meta.chunkDims.z = 128;
    meta.mipLevel = 0;

    vector3d point;
    point.x = 100;
    point.y = 100;
    point.z = 100;

    std::set<int32_t> ids;
    std::cout << "Getting Seg ids." << std::endl;
    client.get_seg_ids(ids, meta, point, 10, viewType::XY_VIEW);

    int32_t segId = *ids.begin();

    bbox bounds;
    std::cout << "Getting Seg bbox." << std::endl;
    client.get_seg_bbox(bounds, meta, segId);

    std::cout << bounds << std::endl;

//    std::cout << "SegID: " << segId << std::endl;
    bbox segBbox;
    segBbox.min = meta.bounds.min;
    segBbox.max.x = segBbox.max.y = segBbox.max.z = 20;

    std::cout << "Getting Seg tiles." << std::endl;
    client.get_seg_tiles(tiles, meta, segId, segBbox, viewType::XY_VIEW);

    vector3i chunk;
    chunk.x = chunk.y = chunk.z = 0;

    std::cout << "Getting Mesh" << std::endl;

    std::string uuid;
    client.get_mesh(uuid,
                    std::string("/omniData/e2198/e2198_a_s8_101_46_e16_116_61.omni.files/segmentations/segmentation1/meshes/"),
                    chunk,
                    segId);

//    testClient::wholeVolume(client);
    transport->close();

    return 0;
}
