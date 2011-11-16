#include "thrift/server.h"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

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

    tile tile;

    metadata metadata;
    metadata.uri = std::string("project.omni.files/channels/channel1/0/volume.uint8_t.raw");
    metadata.bounds.min.x = 0;
    metadata.bounds.min.y = 0;
    metadata.bounds.min.z = 0;
    metadata.bounds.max.x = 150;
    metadata.bounds.max.y = 150;
    metadata.bounds.max.z = 150;
    metadata.resolution.x = 1;
    metadata.resolution.y = 1;
    metadata.resolution.z = 1;
    metadata.type = dataType::UINT8;
    metadata.chunkDims.x = 128;
    metadata.chunkDims.y = 128;
    metadata.chunkDims.z = 128;

    vector3d point;
    point.x = 0;
    point.y = 0;
    point.z = 0;

    client.get_chan_tile(tile, metadata, point, 0, viewType::XY_VIEW);

//    testClient::wholeVolume(client);
    transport->close();

    return 0;
}
