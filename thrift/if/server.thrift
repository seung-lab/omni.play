include "fb303.thrift"

namespace cpp om.server

struct vector3d
{
    1: double x,
    2: double y,
    3: double z
}

struct vector3i
{
    1: i32 x,
    2: i32 y,
    3: i32 z
}

struct rangei
{
	1: i32 from,
	2: i32 to
}

struct bbox
{
    1: vector3d min,
    2: vector3d max
}

struct segData
{
    1: bbox bounds,
    2: i32 size
}

enum viewType
{
    XY_VIEW = 1,
    XZ_VIEW = 2,
    ZY_VIEW = 3
}

struct tile
{
    1: viewType view,
    2: bbox bounds,
    3: binary data
}

struct result
{
    1: list<i32> selected
}

enum dataType {
    INT8 = 1,
    UINT8 = 2,
    INT32 = 3,
    UINT32 = 4,
    FLOAT = 5
}

enum volType {
    CHANNEL = 1,
    SEGMENTATION = 2
}

struct metadata
{
    1: string uri,
    2: bbox bounds,
    3: vector3i resolution,
    4: dataType type,
    5: vector3i chunkDims,
    6: volType vol_type
}

service server extends fb303.FacebookService
{
    map<string, tile> get_tiles( 1: metadata vol, 2: vector3i chunk, 3: viewType view, 4: i32 mipLevel, 5: rangei depths ),

    map<i32, segData> get_seg_list_data( 1: metadata vol, 2: set<i32> segIds ),

    bool modify_global_mesh_data( 1: metadata vol,
    					     	  2: set<i32> addedSegIds,
    					     	  3: set<i32> deletedSegIds,
    					     	  4: i32 segId),

    binary get_mesh( 1: metadata vol,
                     2: vector3i chunk,
                     3: i32 mipLevel,
                     4: i32 segId),

    string get_obj( 1: metadata vol,
                    2: vector3i chunk,
                    3: i32 mipLevel,
                    4: i32 segId)

    list<map<i32, i32>> get_seeds( 1: metadata taskVolume,
		                           2: set<i32> selected,
		                           3: metadata adjacentVolume)
}
