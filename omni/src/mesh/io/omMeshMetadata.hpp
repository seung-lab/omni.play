#ifndef OM_MESH_METADATA_HPP
#define OM_MESH_METADATA_HPP

#include "volume/omSegmentation.h"
#include "datalayer/fs/omFileNames.hpp"

class OmMeshMetadata {
private:
	OmSegmentation *const segmentation_;
	const QString fnp_;

	int fileVersion_;

	bool meshBuilt_;

	// 1 == hdf5
	// 2 == per-chunk files
	int meshVersion_;

	zi::mutex lock_;

public:
	OmMeshMetadata(OmSegmentation *segmentation)
		: segmentation_(segmentation)
		, fnp_(filePath())
		, fileVersion_(1)
		, meshBuilt_(false)
		, meshVersion_(0)
	{}

	bool Load()
	{
		zi::guard g(lock_);
		return load();
	}

	bool IsBuilt() const
	{
		zi::guard g(lock_);
		return meshBuilt_;
	}

	bool IsHDF5() const
	{
		zi::guard g(lock_);
		if(!meshVersion_){
			throw OmIoException("meshes not yet built");
		}
		return 1 == meshVersion_;
	}

	void SetMeshedAndStorageAsHDF5()
	{
		zi::guard g(lock_);
		meshBuilt_ = true;
		meshVersion_ = 1;
		store();
	}

	void SetMeshedAndStorageAsChunkFiles()
	{
		zi::guard g(lock_);
		meshBuilt_ = true;
		meshVersion_ = 2;
		store();
	}

private:
	QString filePath()
	{
		const QString volPath =
			OmFileNames::MakeMeshFolderPath(segmentation_);
		const QString fullPath = QString("%1meshMetadata.ver1")
			.arg(volPath);

		return fullPath;
	}

	bool load()
	{
		QFile file(fnp_);

		if(!file.exists()){
			return false;
		}

		if(!file.open(QIODevice::ReadOnly)){
			throw OmIoException("error reading file", fnp_);
		}

		QDataStream in(&file);
		in.setByteOrder( QDataStream::LittleEndian );
		in.setVersion(QDataStream::Qt_4_6);

		in >> fileVersion_;
		in >> meshBuilt_;
		in >> meshVersion_;

		if(!in.atEnd()){
			throw OmIoException("corrupt file?", fnp_);
		}

		return true;
	}

	void store()
	{
		QFile file(fnp_);

		if (!file.open(QIODevice::WriteOnly)) {
			throw OmIoException("could not write file", fnp_);
		}

		QDataStream out(&file);
		out.setByteOrder( QDataStream::LittleEndian );
		out.setVersion(QDataStream::Qt_4_6);

		out << fileVersion_;
		out << meshBuilt_;
		out << meshVersion_;
	}
};

#endif
