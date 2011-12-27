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
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

    serverClient client(protocol);
    transport->open();

    metadata meta1 = makeMetadata(0,0,0);
    metadata meta2 = makeMetadata(0,0,1);

    int32_t segs[] = { 38, 168, 207, 325, 413, 428, 429, 481, 579, 594,
                       597, 602, 603, 604, 627, 664, 688, 689, 713, 714,
                       755, 788, 806, 811, 848, 886, 900, 901, 920, 921,
                       922, 958, 967, 968, 1009, 1010, 1048, 1059, 1100, 1102,
                       1126, 1162, 1177, 1199, 1221, 1224, 1244, 1246, 1247, 1250,
                       1261, 1299, 1304, 1315, 1351, 1353, 1368, 1369, 1370, 1379,
                       1394, 1412, 1425, 1431, 1433, 1434, 1447, 1450, 1451, 1452,
                       1453, 1466, 1474, 1475, 1476, 1491, 1512, 1515, 1533, 1547,
                       1550, 1557, 1565, 1571, 1576, 1590, 1619, 1625, 1629, 1642,
                       1644, 1645, 1646, 1657, 1658, 1686, 1688, 1698, 1700, 1710,
                       1737, 1739, 1752, 1755, 1767, 1768, 1774, 1784, 1786, 1798,
                       1800, 1801, 1815, 1816, 1830, 1833, 1834, 1840, 1841, 1864,
                       1870, 1871, 1896, 1905, 1921, 1922, 1944, 1955, 1987, 1989,
                       2001, 2018, 2027, 2043, 2045, 2053, 2067, 2068, 2080, 2082,
                       2087, 2099, 2101, 2103, 2148, 2150, 2153, 2167, 2196, 2215,
                       2235, 2262, 2264, 2265, 2277, 2278, 2279, 2282, 2303, 2304,
                       2305, 2338, 2339, 2353, 2369, 2384, 2387, 2403, 2420, 2425,
                       2464, 2478, 2479, 2480, 2497, 2511, 2512, 2513, 2525, 2545,
                       2556, 2572, 2595, 2597, 2616, 2629, 2632, 2647, 2659, 2660,
                       2679, 2707, 2708, 2736, 2765, 2766, 2813, 2843, 2844, 2900,
                       2922, 2934, 2949, 3008, 3009, 3014, 3031, 3043, 3044, 3093,
                       3094, 3121, 3155, 3181, 3192, 3193, 3197, 3213, 3245, 3246,
                       3253, 3277, 3311, 3333, 3334, 3370, 3383, 3386, 3410, 3458,
                       3478, 3510, 3537, 3562, 3574, 3575, 3576, 3577, 3601, 3624,
                       3625, 3626, 3627, 3628, 3642, 3655, 3663, 3664, 3680, 3681,
                       3684, 3694, 3706, 3709, 3711, 3722, 3759, 3788, 3789, 3802,
                       3807, 3811, 3823, 3835, 3838, 3854, 3862, 3865, 3869, 3878,
                       3895, 3896, 3897, 3908, 3917, 3920, 3943, 3979, 3989, 3998,
                       4023, 4048, 4049, 4062, 4066, 4100, 4111, 4131, 4134, 4163 };

    set<int32_t> selected;
    selected.insert(segs, &segs[290]);

    vector<set<int32_t> > result;
    cout << "Getting Seeds 1-2." << endl;
    client.get_seeds(result, meta1, selected, meta2);

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
