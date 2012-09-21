namespace cpp zi.mesh

struct vector3i
{
    1: i32 x,
    2: i32 y,
    3: i32 z
}

struct vector4i
{
    1: i32 x,
    2: i32 y,
    3: i32 z,
    4: i32 a
}

struct meshCoordinate
{
    1: i32 x,
    2: i32 y,
    3: i32 z,
    4: i32 mip,
    5: i32 segID
}

struct meshData
{
    1: binary data
}

struct meshDataResult
{
    1: i32 code,
    2: meshData
}

service RealTimeMesher
{
    bool queueUpdateChunk(
      1: string uri,
      2: vector3i chunk,
      3: binary data,
      ),

    bool updateChunk(
      1: string uri,
      2: vector3i chunk,
      4: binary data,
      ),

    bool queueUpdate(
      1: string uri,
      2: vector3i location,
      3: vector3i size,
      4: binary data,
      ),

    bool update(
      1: string uri,
      2: vector3i location,
      3: vector3i size,
      4: binary data,
      ),

    bool remesh(
        1: bool sync
    ),

    meshDataResult getMesh(
      1: string uri,
      2: meshCoordinate coordinate
    ),

    list<meshDataResult> getMeshes(
      1: string uri,
      2: list<meshCoordinate> coordinates
    ),

    meshDataResult getMeshIfNewer(
      1: string uri,
      2: meshCoordinate coordinate,
      3: i64 version
    ),

    list<meshDataResult> getMeshesIfNewer(
      1: string uri,
      2: list<meshCoordinate> coordinates
      3: list<i64> versions
    ),

    i64 getMeshVersion(
      1: string uri,
      2: meshCoordinate coordinate
    ),

    list<i64> getMeshVersions(
      1: string uri,
      2: list<meshCoordinate> coordinates
    ),

}