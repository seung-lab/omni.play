#include "thrift/server.h"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <vector>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace om::server;

//#include "wholeVolume.hpp"
#include "throws.hpp"

#include <cstdlib>
#include <time.h>


int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    serverClient client(protocol);
    transport->open();

    std::vector<tile> tiles;

    metadata metadata;
    metadata.uri = std::string("/omniData/e2198/e2198_a_s8_101_46_e16_116_61.omni.files/channels/channel1/0/volume.float.raw");
    metadata.bounds.min.x = 0;
    metadata.bounds.min.y = 0;
    metadata.bounds.min.z = 0;
    metadata.bounds.max.x = 150;
    metadata.bounds.max.y = 150;
    metadata.bounds.max.z = 150;
    metadata.resolution.x = 1;
    metadata.resolution.y = 1;
    metadata.resolution.z = 1;
    metadata.type = dataType::FLOAT;
    metadata.chunkDims.x = 128;
    metadata.chunkDims.y = 128;
    metadata.chunkDims.z = 128;

    vector3d point;
    point.x = 20;
    point.y = 0;
    point.z = 0;

    int32_t segId = client.get_seg_id(metadata, metadata.bounds.min);
    bbox segBbox = metadata.bounds;

    client.get_seg_tiles(tiles, metadata, 10, segBbox, 0, viewType::XY_VIEW);

//    testClient::wholeVolume(client);
    transport->close();

    return 0;
}
