#include "thrift/server.h"
#include "zi/for_each.hpp"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

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

metadata makeMetadata(int x, int y, int z, volType::type type)
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
    meta.vol_type = type;

    return meta;
}

int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("brainiac.mit.edu", 9090));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    serverClient client(protocol);
    transport->open();

    vector3i chunk;
    chunk.x = chunk.y = chunk.z = 0;

    std::string path = "/usr/local/omni/data/omelette2/x07/y50/x07y50z02_s1811_11475_659_e2066_11730_914.omni.files/segmentations/segmentation1/meshes/";

    std::string obj;

    client.get_obj(obj, path, chunk, 428);
    std::cout << obj;

    transport->close();

    return 0;
}
