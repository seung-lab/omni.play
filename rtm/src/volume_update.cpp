#include <cstddef>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

namespace zi {
namespace mesh {

inline void volume_update(int32_t x, int32_t y, int32_t z,
                          int32_t w, int32_t h, int32_t d)
{
    int32_t x0 = x;
    int32_t y0 = y;
    int32_t z0 = z;
    int32_t x1 = x + w - 1;
    int32_t y1 = y + h - 1;
    int32_t z1 = z + d - 1;

    int32_t mx0 = x0 / 128;
    int32_t my0 = y0 / 128;
    int32_t mz0 = z0 / 128;
    int32_t mx1 = x1 / 128;
    int32_t my1 = y1 / 128;
    int32_t mz1 = z1 / 128;

    for ( int32_t i = mx0; i <= mx1; ++i )
    {
        for ( int32_t j = my0; j <= my1; ++j )
        {
            for ( int32_t k = mz0; k <= mz1; ++k )
            {
                int32_t fromx = std::max( x0, i * 128 );
                int32_t tox   = std::min( x1, (i+1) * 128 - 1 );

                int32_t fromy = std::max( y0, j * 128 );
                int32_t toy   = std::min( y1, (j+1) * 128 - 1 );

                int32_t fromz = std::max( z0, k * 128 );
                int32_t toz   = std::min( z1, (k+1) * 128 - 1 );

                std::cout << "Update MIP0; " << i << ' ' << j << ' ' << k
                          << "\n"
                          << "    [" << fromx << ' ' << tox << "]\n"
                          << "    [" << fromy << ' ' << toy << "]\n"
                          << "    [" << fromz << ' ' << toz << "]\n";

                // load appropriate MIP volume
                // update appropriate subvolume
                // save the volume
            }
        }
    }
}


} // namespace mesh
} // namespace zi

int main()
{
    zi::mesh::volume_update(15, 0, 127, 10, 300, 128);
}
