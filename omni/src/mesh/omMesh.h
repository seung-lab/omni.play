#ifndef OM_MIP_MESH_H
#define OM_MIP_MESH_H

/**
 *  The mesh data associated with the region of a segment enclosed
 *   by a specific MipChunk.
 *
 *  Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "common/omCommon.h"
#include "common/omGl.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMeshCoord.h"

class OmDataForMeshLoad;
class OmMeshCache;
class OmMeshManager;
class OmSegmentation;

class OmMesh {
public:
    OmMesh(OmSegmentation*, const OmMeshCoord&,
              OmMeshManager*, OmMeshCache*);

    virtual ~OmMesh();

    void Load();

    bool HasData() const;
    uint64_t NumBytes() const;

    void Draw();

    OmDataForMeshLoad* Data(){
        return data_.get();
    }

private:
    OmSegmentation *const segmentation_;
    OmMeshCache *const cache_;
    OmMeshManager *const meshMan_;

    const OmMeshCoord meshCoord_;

    om::shared_ptr<OmDataForMeshLoad> data_;

    GLuint vertexDataVboId_;
    GLuint vertexIndexDataVboId_;
    bool isVbo();
    void createVbo();
    void deleteVbo();
    GLuint createVbo(const void *data, int dataSize,
                     GLenum target, GLenum usage);

    boost::optional<GLuint> displayList_;
    void makeDisplayList();

    uint64_t numBytes_;
    bool hasData_;
};

#endif
