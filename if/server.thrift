namespace cpp om.common

struct vector3d
{
    1: double x,
    2: double y,
    3: double z
}

struct tile
{
    1: binary data
}

struct click_info
{
    1: i32 segId,
    2: tile tile_data
}

struct result
{
    1: list<i32> selected
}

service server
{
    tile get_chan_tile( 1: vector3d point ),
    tile get_seg_tile( 1: vector3d point ),
    click_info click( 1: vector3d point ),
    double compare_results( 1: list<result> old_results, 2: result new_result)
}
