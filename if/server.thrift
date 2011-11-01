namespace cpp om.common

struct vector3d
{
    1: double x,
    2: double y,
    3: double z
}

struct bbox
{
  1: vector3d min,
  2: vector3d max
}

struct tile
{
    1: binary data
}

struct result
{
    1: list<i32> selected
}

service server
{
    bbox get_volume_bounds(),
    tile get_chan_tile( 1: vector3d point, 2: i32 mipLevel ),
    tile get_seg_tile( 1: vector3d point, 2: i32 mipLevel, 3: i32 segId ),
    bbox get_seg_bbox( 1: i32 segId ),
    i32 get_seg_id( 1: vector3d point ),
    list<i32> get_seg_ids(1: vector3d point, 2: double radius ),
    double compare_results( 1: list<result> old_results, 2: result new_result)
}
