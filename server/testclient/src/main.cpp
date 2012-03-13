#include "thrift/server.h"
#include "zi/for_each.hpp"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <boost/lexical_cast.hpp>

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
    boost::shared_ptr<TSocket> socket(new TSocket("hebb", 9090));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    serverClient client(protocol);
    transport->open();

    metadata meta = makeMetadata(0,0,0, volType::SEGMENTATION);

    std::vector<edge> mst;
    client.get_mst(mst, meta);

    FOR_EACH(e, mst)
    {
    	std::cout << e->a << " - " << e->b << ": " << e->value << std::endl;
    }

    transport->close();

    return 0;
}

/*  Dump obj to disk
int main(int argc, char **argv) {
    boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    serverClient client(protocol);
    transport->open();

    std::vector<uint32_t> all_ids;
    //all_ids.push_back(10005);
    //all_ids.push_back(10018);
    //all_ids.push_back(40001);
    //all_ids.push_back(40005);
    //all_ids.push_back(60002);
    //all_ids.push_back(80001);
    //all_ids.push_back(90001);
    //all_ids.push_back(91001);
    //all_ids.push_back(91002);
    //all_ids.push_back(91003);
    all_ids.push_back(91004);
    //all_ids.push_back(91005);

    std::string path = "/omelette/2/omniData/e2198_all/cells.omni.files/segmentations/segmentation1/meshes/";

    vector3i chunk;

//    FOR_EACH( it, all_ids )
    {
        for ( int i = 0; i <= 18; ++i )
        {
            for ( int j = 0; j <= 81; ++j )
            {
                for ( int k = 0; k <= 50; ++k )
		FOR_EACH (it, all_ids)
                {
                    chunk.x = i;
                    chunk.y = j;
                    chunk.z = k;
                    std::string r;                    
                    try {
                        client.get_obj(r, path, chunk, 0, *it); // 0 IS THE MIP LEVEL
                    }
                    catch ( std::exception e )
                    {
                        std::cout << "NO MESH (Exception) for ID: " << *it
                            << " chunk: " << chunk.x << ' ' << chunk.y << ' ' << chunk.z << std::endl;
                        continue;
                    }
                    if ( r != "" )
                    {
                        std::string filename = boost::lexical_cast<std::string>(*it) + "."
                            + boost::lexical_cast<std::string>(i) + "."
                            + boost::lexical_cast<std::string>(j) + "."
                            + boost::lexical_cast<std::string>(k) + ".obj";
                        std::cout << "Got mesh for ID: " << *it
                            << " chunk: " << chunk.x << ' ' << chunk.y << ' ' << chunk.z << std::endl;
                        std::ofstream fout(filename.c_str());
                        fout << r;
                    }
                    else
                    {
                        std::cout << "NO MESH for ID: " << *it
                            << " chunk: " << chunk.x << ' ' << chunk.y << ' ' << chunk.z << std::endl;
                        
                    }
                }
            }
        }        
    }
          

    //std::cout << uuid;

    transport->close();

    return 0;
}
*/