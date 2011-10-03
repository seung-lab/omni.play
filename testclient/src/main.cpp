#include "thrift/server.h"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace om::server;

int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    
    serverClient client(protocol);
    transport->open();
    vector3d point;
    tile tile;
    click_info info;
    client.get_chan_tile(tile, point);
    client.get_seg_tile(tile, point);
    client.click(info, point);
    std::vector<result> previous;
    result next;
    client.compare_results(previous, next);
    transport->close();
    
    return 0;
}