#include "mesh.hpp"
#include "types.hpp"
#include "chunk.hpp"
#include "files.hpp"
#include "rtm.hpp"

int main()
{
    zi::mesh::file_io.create_path<4>("pera", zi::mesh::vec4u(1,2,3,4));
    zi::mesh::file_io.remove("pera/asd");

    uint32_t* d = new uint32_t[500*150*150];

    for ( uint32_t i = 0; i < 150*500*150; ++i )
    {
        d[i] = 71;
    }

    zi::mesh::rtm rtm;

    rtm.volume_update( 120, 20, 20, 500, 150, 50,
                       reinterpret_cast<char*>(d));
}
