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

struct value {
    1: required dataType type,
    2: optional byte int8, // for INT8
    3: optional i16 int16, // for UINT8
    4: optional i32 int32, // for INT32
    5: optional i64 int64, // for UINT32
    6: optional double double_float // for FLOAT
}

struct metadata
{
    1: i32 volId,
    2: bbox bounds,
    3: vector3i resolution,
    4: dataType type,
    6: vector3i chunkDims,
}

service server
{
    tile get_tile( 1: metadata vol, 2: vector3d point, 3: viewType view ),

    map<string, tile> get_tiles( 1: metadata vol,
                                 2: bbox bounds,
                                 3: viewType view,
                                 4: optional value filter),

    value get_value( 1: metadata vol, 2: vector3d point),

    set<value> get_values( 1: metadata vol,
                           2: vector3d point,
                           3: i32 radius,
                           4: viewType view),

    bbox get_seg_bbox( 1: metadata vol, 2: i32 segId ),

    string get_mesh( 1: metadata vol, 2: vector3i chunk, 3: i32 segId),

    # do we need the metadata for the comparison procedure?
    double compare_results( 1: list<result> old_results, 2: result new_result),

    list<set<value>> get_seeds( 1: metadata taskVolume,
                                2: set<value> selected,
                                3: metadata adjacentVolume),

    oneway void make_volume( 1: metadata vol, 2: string uri)
}
