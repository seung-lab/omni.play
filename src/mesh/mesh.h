#pragma once

/**
 *  The mesh data associated with the region of a segment enclosed
 *   by a specific MipChunk.
 *
 *  Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "common/common.h"
#include "common/omGl.h"
#include "mesh/meshTypes.h"
#include "mesh/meshCoord.h"

class dataForMeshLoad;
class meshCache;
class meshManager;
class segmentation;
class QGLContext;

class mesh {
public:
    mesh(segmentation*, const meshCoord&,
           meshManager*, meshCache*);

    virtual ~mesh();

    void Load();

    bool HasData() const;
    uint64_t NumBytes() const;

    void Draw(QGLContext const* context);

    dataForMeshLoad* Data(){
        return data_.get();
    }

private:
    segmentation *const segmentation_;
    meshCache *const cache_;
    meshManager *const meshMan_;

    const meshCoord meshCoord_;

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

