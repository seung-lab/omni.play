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

struct edge
{
    1: i32 a,
    2: i32 b,
    3: double value
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
    6: i32 mipLevel,
    7: volType vol_type
}

service server
{
    void add_chunk( 1: metadata vol, 2: vector3i chunk, 3: binary data ),
    void delete_chunk( 1: metadata vol, 2: vector3i chunk ),
    binary get_chunk( 1: metadata vol, 2: vector3i chunk ),
    list<edge> get_graph( 1: metadata vol ),
    list<edge> get_mst( 1: metadata vol ),

    metadata create_segmentation( 1: metadata chan,
                                  2: i32 newVolId,
                                  3: list<string> features ),

    tile get_chan_tile( 1: metadata vol, 2: vector3d point, 3: viewType view ),

    map<string, tile> get_seg_tiles( 1: metadata vol,
                                     2: i32 segId,
                                     3: bbox segBbox,
                                     4: viewType view ),

    i32 get_seg_id( 1: metadata vol, 2: vector3d point),

    segData get_seg_data( 1: metadata vol,
                          2: i32 segId ),

    set<i32> get_seg_ids( 1: metadata vol,
                          2: vector3d point,
                          3: i32 radius,
                          4: viewType view),

    string get_mesh( 1: string uri,
                     2: i32 segId)

    string get_obj( 1: string uri,
                    2: vector3i chunk,
                    3: i32 segId)

    # do we need the metadata for the comparison procedure?
    double compare_results( 1: list<result> old_results, 2: result new_result)

    list<set<i32>> get_seeds( 1: metadata taskVolume,
                              2: set<i32> selected,
                              3: metadata adjacentVolume)
}
