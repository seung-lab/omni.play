namespace cpp bint

struct server_id
{
    1: string   address,
    2: i32      port
}


service storage_manager
{
    server_id get_server (
      1: string key
    ),

    bool register_server (
      1: server_id id
    ),

    void free_key (
      1: server_id id,
      2: string key
    ),

    map<string,string> get_stats (
    ),

    void server_full (
      1: server_id id
    ),

    map<server_id, list<string> > multi_get (
      1: list<string> keys
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

    map<string,string> get_stats(
    ),

    bool remove (
      1: string key
    ),

    map<string,binary> multi_get (
      1: list<string> keys
    ),
}