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

struct bbox
{
    1: vector3d min,
    2: vector3d max
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

struct metadata
{
    1: string uri,
    2: bbox bounds,
    3: vector3i resolution,
    4: dataType type,
    5: vector3i chunkDims,
    6: i32 mipLevel
}

service server
{
    tile get_chan_tile( 1: metadata vol, 2: vector3d point, 3: viewType view ),

    map<string, tile> get_seg_tiles( 1: metadata vol,
                                     2: i32 segId,
                                     3: bbox segBbox,
                                     4: viewType view ),

    i32 get_seg_id( 1: metadata vol, 2: vector3d point),

#    bbox get_seg_bbox( 1: i32 segId ), #Just a lookup... store in the DB?

    list<i32> get_seg_ids( 1: metadata vol,
                           2: vector3d point,
                           3: double radius,
                           4: viewType view),

    string get_mesh( 1: string uri,
                     2: vector3i chunk,
                     3: i32 segId)

    # do we need the metadata for the comparison procedure?
    double compare_results( 1: list<result> old_results, 2: result new_result)
}
