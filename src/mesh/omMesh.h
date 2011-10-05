#pragma once

/**
 *  The mesh data associated with the region of a segment enclosed
 *   by a specific MipChunk.
 *
 *  Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "common/common.h"
#include "common/omGl.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMeshCoord.h"

class dataForMeshLoad;
class OmMeshCache;
class OmMeshManager;
class segmentation;
class QGLContext;

class OmMesh {
public:
    OmMesh(segmentation*, const OmMeshCoord&,
           OmMeshManager*, OmMeshCache*);

    virtual ~OmMesh();

    void Load();

    bool HasData() const;
    uint64_t NumBytes() const;

    void Draw(QGLContext const* context);

    dataForMeshLoad* Data(){
        return data_.get();
    }

private:
    segmentation *const segmentation_;
    OmMeshCache *const cache_;
    OmMeshManager *const meshMan_;

    const OmMeshCoord meshCoord_;

    QGLContext const* context_;

    om::shared_ptr<dataForMeshLoad> data_;

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

