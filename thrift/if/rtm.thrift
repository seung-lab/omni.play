namespace cpp zi.mesh

struct Vector3i
{
    1: i32 x,
    2: i32 y,
    3: i32 z
}

struct Vector4i
{
    1: i32 x,
    2: i32 y,
    3: i32 z,
    4: i32 a
}

struct MeshCoordinate
{
    1: i32 x,
    2: i32 y,
    3: i32 z,
    4: i32 mip,
    5: i32 segID
}

struct MeshData
{
    1: binary data,
    2: i64    hash
}

enum MeshResultCode
{
	MISSING = 0,
	FOUND = 1,
	NOT_UPDATED = 2
}

struct MeshDataResult
{
    1: MeshResultCode code,
    2: MeshData mesh,
}

service RealTimeMesher
{
    bool updateChunk(
      1: string uri,
      2: Vector3i chunk,
      4: binary data,
      ),

    bool update(
      1: string uri,
      2: Vector3i location,
      3: Vector3i size,
      4: binary data,
      ),

    bool maskedUpdate(
      1: string uri,
      2: Vector3i location,
      3: Vector3i size,
      4: binary data,
      5: binary mask,
      ),

    MeshDataResult getMesh(
      1: string uri,
      2: MeshCoordinate coordinate
    ),

    list<MeshDataResult> getMeshes(
      1: string uri,
      2: list<MeshCoordinate> coordinates
    ),

    MeshDataResult getMeshIfNewer(
      1: string uri,
      2: MeshCoordinate coordinate,
      3: i64 version
    ),

    list<MeshDataResult> getMeshesIfNewer(
      1: string uri,
      2: list<MeshCoordinate> coordinates
      3: list<i64> versions
    ),

    i64 getMeshVersion(
      1: string uri,
      2: MeshCoordinate coordinate
    ),

    list<i64> getMeshVersions(
      1: string uri,
      2: list<MeshCoordinate> coordinates
    ),

    void clear(
      1: string uri
    )

}