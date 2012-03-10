#pragma once

#include "common/om.hpp"
#include "mesh/meshTypes.h"

#include <boost/scoped_ptr.hpp>

class meshCache;
class meshConvertV1toV2;
class meshFilePtrCache;
class meshMetadata;
class meshReader;
class mesh;
class meshCoord;
class segmentation;

#include <QDataStream>

class mesh::manager {
public:
    mesh::manager(segmentation* segmentation,
                  const double threshold);
    ~mesh::manager();

    void Create();
    void Load();

    segmentation* GetSegmentation() const{
        return segmentation_;
    }

    double Threshold() const {
        return threshold_;
    }

    void CloseDownThreads();

    meshPtr Produce(const meshCoord&);

    void GetMesh(meshPtr&, const meshCoord&, const om::Blocking);

    void UncacheMesh(const meshCoord& coord);

    void ClearCache();

    meshReader* Reader(){
        return reader_.get();
    }

    meshMetadata* Metadata(){
        return metadata_.get();
    }

    void ActivateConversionFromV1ToV2();

    meshConvertV1toV2* Converter(){
        return converter_.get();
    }

    meshFilePtrCache* FilePtrCache(){
        return filePtrCache_.get();
    }

private:
    segmentation *const segmentation_;
    const double threshold_;

    const boost::scoped_ptr<meshCache> dataCache_;
    const boost::scoped_ptr<meshFilePtrCache> filePtrCache_;
    const boost::scoped_ptr<meshMetadata> metadata_;

    boost::scoped_ptr<meshReader> reader_;
    boost::scoped_ptr<meshConvertV1toV2> converter_;

    void HandleFetchUpdate();

    void inferMeshMetadata();
    void loadThreadhold1();
    void loadThreadholdNon1();
    void moveOldMetadataFile();

    friend QDataStream &operator<<(QDataStream&, const mesh::manager&);
    friend QDataStream &operator>>(QDataStream&, mesh::manager&);
};

