#ifndef OM_MESH_CHUNK_TYPES_H
#define OM_MESH_CHUNK_TYPES_H

struct OmMeshFilePart{
	uint64_t offsetIntoFile;
	uint64_t numElements;
	uint64_t totalBytes;
};

struct OmMeshDataEntry {
	OmSegID segID;
	OmMeshFilePart vertexIndex;
	OmMeshFilePart vertexData;
	OmMeshFilePart stripData;
};

#endif
