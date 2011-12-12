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
    const int y_min = 14866;
    const int z_min = 6162;
    const int offset = 192;
    const int chunk_size = 128;
    const int num_chunks = 2;

    metadata meta;
    meta.bounds.min.x = x_min + (x - 1) * offset;
    meta.bounds.min.y = y_min + (y - 1) * offset;
    meta.bounds.min.z = z_min + (z - 1) * offset;
    meta.bounds.max.x = meta.bounds.min.x + num_chunks * chunk_size;
    meta.bounds.max.y = meta.bounds.min.y + num_chunks * chunk_size;
    meta.bounds.max.z = meta.bounds.min.z + num_chunks * chunk_size;
    stringstream ss;
    ss << "/omniData/omniWebTest.copy/" << x << y << z << "_s"
       << meta.bounds.min.x + 1 << "_" << meta.bounds.min.y + 1 << "_" << meta.bounds.min.z + 1 << "_e"
       << meta.bounds.max.x << "_" << meta.bounds.max.y << "_" << meta.bounds.max.z << ".omni.files";
    meta.uri = ss.str();
    meta.resolution.x = meta.resolution.y = meta.resolution.z = 1;
    meta.type = dataType::UINT32;
    meta.chunkDims.x = meta.chunkDims.y = meta.chunkDims.z = chunk_size;
    meta.mipLevel = 0;

    return meta;
}

int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    serverClient client(protocol);
    transport->open();

    metadata meta1 = makeMetadata(1,1,1);
    metadata meta2 = makeMetadata(2,1,1);
    metadata meta3 = makeMetadata(1,2,1);
    metadata meta4 = makeMetadata(1,1,2);

    set<int32_t> selected;
    selected.insert(908);
    selected.insert(3112);
    selected.insert(2507);
    selected.insert(502);
    selected.insert(930);

    vector<set<int32_t> > result;
    cout << "Getting Seeds 1-2." << endl;
    client.get_seeds(result, meta1, selected, meta2);

    vector<set<int32_t> > result2;
    cout << "Getting Seeds 1-3." << endl;
    client.get_seeds(result2, meta1, selected, meta3);

    vector<set<int32_t> > result3;
    cout << "Getting Seeds 1-4." << endl;
    client.get_seeds(result3, meta1, selected, meta4);

    FOR_EACH(seed, result)
    {
        cout << "Seed: " << endl;
        FOR_EACH(seg, *seed) {
            cout << *seg << ", ";
        }
        cout << endl;
    }

//    testClient::wholeVolume(client);
    transport->close();

    return 0;
}
