namespace cpp om.server

struct server_id
{
    1: string   address,
    2: i32      port
}


service storage_manager
{
    server_id get_server(
      1: string key
    ),

    bool register_server(
      1: server_id id
    ),
}

service storage_server
{
    binary get (
      1: string key,
    ),

    bool put (
      1: string key,
      2: binary value,
    ),

    map<string,i64> get_stats(
    ),
}