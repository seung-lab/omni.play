#pragma once

#include "common/common.h"
#include "common/omGl.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMeshCoord.h"

class OmDataForMeshLoad;
class OmMeshCache;
class OmMeshManager;
class OmSegmentation;
class QGLContext;

class OmMesh {
public:
    OmMesh(OmSegmentation*, const OmMeshCoord&,
           OmMeshManager*, OmMeshCache*);

    virtual ~OmMesh();

    void Load();

    bool HasData() const;
    uint64_t NumBytes() const;

    void Draw(QGLContext const* context);

    OmDataForMeshLoad* Data(){
        return data_.get();
    }

private:
    OmSegmentation *const segmentation_;
    OmMeshCache *const cache_;
    OmMeshManager *const meshMan_;

    const OmMeshCoord meshCoord_;

    QGLContext const* context_;

    boost::shared_ptr<OmDataForMeshLoad> data_;

    GLuint vertexDataVboId_;
    GLuint vertexIndexDataVboId_;
    bool isVbo();
    bool createVbo();
    void deleteVbo();
    GLuint createVbo(const void *data, int dataSize,
                     GLenum target, GLenum usage);

    boost::optional<GLuint> displayList_;
    void makeDisplayList(QGLContext const* context);

    uint64_t numBytes_;
    bool hasData_;
};

