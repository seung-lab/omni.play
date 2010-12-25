#ifndef OM_MIP_MESH_MANAGER_H
#define OM_MIP_MESH_MANAGER_H

#include "mesh/omMeshTypes.h"

class OmMeshCache;
class OmMeshConvertV1toV2;
class OmMeshFilePtrCache;
class OmMeshMetadata;
class OmMeshReaderV2;
class OmMipMesh;
class OmMipMeshCoord;
class OmSegmentation;

class OmMipMeshManager {
public:
	OmMipMeshManager(OmSegmentation* segmentation);
	~OmMipMeshManager();

	void Load();
	void InferMeshMetadata();

	OmSegmentation* GetSegmentation() const{
		return segmentation_;
	}

	void CloseDownThreads();

	OmMipMeshPtr Produce(const OmMipMeshCoord&);
	void GetMesh(OmMipMeshPtr& mesh, const OmMipMeshCoord& coord);
	void UncacheMesh(const OmMipMeshCoord& coord);

	OmMeshReaderV2* Reader(){
		return reader_.get();
	}

	OmMeshMetadata* Metadata(){
		return metadata_.get();
	}

	void ActivateConversionFromV1ToV2();

	OmMeshConvertV1toV2* Converter(){
		return converter_.get();
	}

	OmMeshFilePtrCache* FilePtrCache(){
		return filePtrCache_.get();
	}

private:
	OmSegmentation *const segmentation_;
	OmMeshCache *const mDataCache;

	boost::shared_ptr<OmMeshFilePtrCache> filePtrCache_;
	boost::shared_ptr<OmMeshMetadata> metadata_;
	boost::shared_ptr<OmMeshReaderV2> reader_;
	boost::shared_ptr<OmMeshConvertV1toV2> converter_;

	void HandleFetchUpdate();

	friend QDataStream &operator<<(QDataStream&, const OmMipMeshManager&);
	friend QDataStream &operator>>(QDataStream&, OmMipMeshManager&);
};

#endif
