namespace cpp bint

service storage_server {

    binary get (
      1: string key,
    ),

    bool put (
      1: string key,
      2: binary value,
    ),
}