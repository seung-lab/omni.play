#pragma once

#include "thrift/server.h"
#include <cstdlib>
#include <time.h>

std::ostream& operator<< (std::ostream& stream, const vector3d& p) {
    return stream << p.x << ", " << p.y << ", " << p.z;
}


class testClient {
public:
    static void getChanTiles(serverClient client, bbox bounds, int mipLevel, int skip)
    {
        vector3d point = bounds.min;
        tile tile;
        int step = 128 * (2 << mipLevel) * skip;
        for(; point.x < bounds.max.x; point.x += step)
        {
            for(; point.y < bounds.max.y; point.y += step)
            {
                for(; point.z < bounds.max.z; point.z += step)
                {
                    client.get_chan_tile(tile, point, mipLevel);
                }
                point.z = bounds.min.z;
            }
            point.y = bounds.min.y;
        }
    }

    static void getNRandomSegments(serverClient client, bbox bounds, int mipLevel, int n)
    {
        srand(time(NULL));

        int xsize = bounds.max.x - bounds.min.x;
        int ysize = bounds.max.y - bounds.min.y;
        int zsize = bounds.max.z - bounds.min.z;

        for (int i = 0; i < n; i++)
        {
            vector3d p;
            p.x = rand() % xsize + bounds.min.x;
            p.y = rand() % ysize + bounds.min.y;
            p.z = rand() % zsize + bounds.min.z;

            uint32_t id = client.get_seg_id(p);
            if(id == 0) {
                std::cout << "Found Segment Id 0 at " << p << std::endl;
                continue;
            }

            bbox segBbox;
            client.get_seg_bbox(segBbox, id);

            try
            {
                tile tile;
                client.get_seg_tile(tile, segBbox.min, mipLevel, id);
                vector3d max = segBbox.max;
                client.get_seg_tile(tile, segBbox.max, mipLevel, id);
            } catch(...) {
                std::cout << "Failed Getting Segment tile on :" << std::endl
                          << "\tSegID: " << id << std::endl
                          << "\tbbox min: " << segBbox.min << std::endl
                          << "\tbbox max: " << segBbox.max << std::endl << std::endl;

            }
        }
    }

    static void wholeVolume(serverClient client)
    {
        std::cout << "Getting Bounds:" << std::endl;
        bbox bounds;
        client.get_volume_bounds(bounds);
        std::cout << "\tbbox min: " << bounds.min << std::endl
                  << "\tbbox max: " << bounds.max << std::endl;

        std::cout << "Getting Channel Tiles, Mip(0)" << std::endl;
        getChanTiles(client, bounds, 0, 2);

        std::cout << "Getting Channel Tiles, Mip(1)" << std::endl;
        getChanTiles(client, bounds, 1, 1);

        std::cout << "Getting Segment Tiles, Mip(0)" << std::endl;
        getNRandomSegments(client, bounds, 0, 100);

        std::cout << "Getting Segment Tiles, Mip(1)" << std::endl;
        getNRandomSegments(client, bounds, 1, 100);
    }
};

