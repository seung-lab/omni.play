#include "thrift/server.h"
#include "zi/for_each.hpp"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <vector>
#include <iostream>
#include <sstream>

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

metadata makeMetadata(int x, int y, int z)
{
    const int x_min = 914;
    const int y_min = 15506;
    const int z_min = 6930;
    const int offset = 224;
    const int chunk_size = 128;
    const int num_chunks = 2;

    metadata meta;
    meta.bounds.min.x = x_min + x * offset;
    meta.bounds.min.y = y_min + y * offset;
    meta.bounds.min.z = z_min + z * offset;
    meta.bounds.max.x = meta.bounds.min.x + num_chunks * chunk_size - 1;
    meta.bounds.max.y = meta.bounds.min.y + num_chunks * chunk_size - 1;
    meta.bounds.max.z = meta.bounds.min.z + num_chunks * chunk_size - 1;
    stringstream ss;
    ss << "/home/balkamm/omniData/" << x << y << z << "_s"
       << meta.bounds.min.x + 1 << "_" << meta.bounds.min.y + 1 << "_" << meta.bounds.min.z + 1 << "_e"
       << meta.bounds.max.x + 1 << "_" << meta.bounds.max.y + 1 << "_" << meta.bounds.max.z + 1 << ".omni.files";
    meta.uri = ss.str();
    meta.resolution.x = meta.resolution.y = meta.resolution.z = 1;
    meta.type = dataType::UINT32;
    meta.chunkDims.x = meta.chunkDims.y = meta.chunkDims.z = chunk_size;
    meta.mipLevel = 0;

    return meta;
}

int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    serverClient client(protocol);
    transport->open();

    std::string uri = "/home/balkamm/omniData/001_s915_15507_7155_e1170_15762_7410.omni.files/segmentations/segmentation1/meshes/";
    std::string result;

    vector3i chunk;
    chunk.x = chunk.y = chunk.z = 0;

    client.get_mesh(result, uri, chunk, 12);

    std::cout << result;
    transport->close();

    return 0;
}
