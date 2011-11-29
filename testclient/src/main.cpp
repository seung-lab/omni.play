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

    std::map<std::string, tile> tiles;

    metadata metadata;
    metadata.uri = std::string("/omniData/e2198/e2198_a_s8_101_46_e16_116_61.omni.files/segmentations/segmentation1/0/volume.uint32_t.raw");
    metadata.bounds.min.x = 0;
    metadata.bounds.min.y = 0;
    metadata.bounds.min.z = 0;
    metadata.bounds.max.x = 1151;
    metadata.bounds.max.y = 2047;
    metadata.bounds.max.z = 2047;
    metadata.resolution.x = 1;
    metadata.resolution.y = 1;
    metadata.resolution.z = 1;
    metadata.type = dataType::UINT32;
    metadata.chunkDims.x = 128;
    metadata.chunkDims.y = 128;
    metadata.chunkDims.z = 128;
    metadata.mipLevel = 0;

    vector3d point;
    point.x = 100;
    point.y = 100;
    point.z = 100;

    int32_t segId = client.get_seg_id(metadata, point);

    std::cout << "SegID: " << segId << std::endl;
/*    bbox segBbox;
    segBbox.min = metadata.bounds.min;
    segBbox.max.x = segBbox.max.y = segBbox.max.z = 20;

    client.get_seg_tiles(tiles, metadata, segId, segBbox, viewType::XY_VIEW);

    for(std::map<std::string, tile>::iterator it = tiles.begin(); it != tiles.end(); it++)
    {
        std::cout << it->first << " = " << it->second.bounds << std::endl;
    }
*/
    vector3i chunk;
    chunk.x = chunk.y = chunk.z = 0;

    std::string uuid;
    client.get_mesh(uuid,
                    std::string("/omniData/e2198/e2198_a_s8_101_46_e16_116_61.omni.files/segmentations/segmentation1/meshes/1.0000/0/0/0/0/"),
                    chunk,
                    segId);

//    testClient::wholeVolume(client);
    transport->close();

    return 0;
}
