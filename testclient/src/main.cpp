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

    metadata meta;
    meta.uri = std::string("/omniData/omniWebTest.copy/111_s915_14867_6163_e1170_15122_6418.omni.files");
    meta.bounds.min.x = meta.bounds.min.y = meta.bounds.min.z = 0;
    meta.bounds.max.x = meta.bounds.max.y = meta.bounds.max.z = 255;
    meta.resolution.x = meta.resolution.y = meta.resolution.z = 1;
    meta.type = dataType::UINT32;
    meta.chunkDims.x = meta.chunkDims.y = meta.chunkDims.z = 128;
    meta.mipLevel = 0;

	metadata meta2 = meta;
	meta.uri = std::string("/omniData/omniWebTest.copy/211_s1107_14867_6163_e1362_15122_6418.omni.files");
    
	std::set<int32_t> selected;

	std::vector<std::set<int32_t> > result;

    client.get_seeds(result, meta, selected, meta2);

	FOR_EACH(seed, result)
	{
		std::cout << "Seed: " << std::endl;
		FOR_EACH(seg, *seed) {
			std::cout << *seg << ", ";
		}
		std::cout << std::endl;
	}

//    testClient::wholeVolume(client);
    transport->close();

    return 0;
}
