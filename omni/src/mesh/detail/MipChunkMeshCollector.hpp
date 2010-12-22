#ifndef OMNI_SRC_MESH_DETAIL_MIP_CHUNK_MESH_COLLECTOR_HPP
#define OMNI_SRC_MESH_DETAIL_MIP_CHUNK_MESH_COLLECTOR_HPP 1

#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshCoord.h"
#include "mesh/omMipMeshManager.h"
#include "TriStripCollector.hpp"

#include "mesh/io/v2/omMeshIO.hpp"

#include <zi/bits/cstdint.hpp>
#include <zi/bits/unordered_map.hpp>
#include <zi/bits/shared_ptr.hpp>
#include <zi/vl/vec.hpp>
#include <zi/concurrency/rwmutex.hpp>

#include <vector>

class MipChunkMeshCollector
{
private:
    int                segmentationId_;
    OmMipChunkCoord    mipCoordinate_ ;
    OmMipMeshManager*  mipMeshManager_;
    zi::rwmutex        lock_   ;

    zi::unordered_map< int, zi::shared_ptr< TriStripCollector > > meshes_;

    void save_( int id, zi::shared_ptr< TriStripCollector > strips )
    {
        OmMipMeshCoord mmCoor( mipCoordinate_, id );
        OmMipMeshPtr   oMesh = mipMeshManager_->AllocMesh( mmCoor );

        oMesh->mVertexIndexCount = strips->indicesSize();
        oMesh->mpVertexIndexDataWrap =
            OmDataWrapper< GLuint >::produce
            ( new GLuint[oMesh->mVertexIndexCount], om::NEW_ARRAY );

        oMesh->mVertexCount = strips->dataSize() / 6;
        oMesh->mpVertexDataWrap =
            OmDataWrapper<float>::produce
            ( new GLfloat[ strips->dataSize() ], om::NEW_ARRAY );

        oMesh->mStripCount = strips->stripsSize() / 2;
        oMesh->mpStripOffsetSizeDataWrap =
            OmDataWrapper<uint32_t>::produce
            ( new uint32_t[ strips->stripsSize() ], om::NEW_ARRAY );

        strips->copyTo( oMesh->mpVertexDataWrap->getPtr<float>(),
                        oMesh->mpVertexIndexDataWrap->getPtr<GLuint>(),
                        oMesh->mpStripOffsetSizeDataWrap->getPtr<unsigned int>() );

        oMesh->mTrianCount = 0;
        oMesh->Save();
        strips->clear();
    }

public:
    MipChunkMeshCollector( int seg_id, OmMipChunkCoord coord,
                           OmMipMeshManager* mipMeshMngr )
        : segmentationId_( seg_id ),
          mipCoordinate_( coord ),
          mipMeshManager_( mipMeshMngr ),
          lock_(),
          meshes_()
    {
    }

    void registerMeshPart( int id )
    {
        zi::rwmutex::write_guard g( lock_ );
        if ( meshes_.count( id ) == 0 )
        {
            meshes_.insert( std::make_pair
                            ( id, zi::shared_ptr< TriStripCollector >
                              ( new TriStripCollector )));
        }

        meshes_[ id ]->registerPart();
    }

    zi::shared_ptr< TriStripCollector > getMesh( int id )
    {
        zi::rwmutex::read_guard g( lock_ );

        if ( meshes_.count( id ) == 0 )
        {
            return zi::shared_ptr< TriStripCollector >();
        }

        return meshes_[ id ];
    }

    void save( int id )
    {
        //std::cout << "Saving: " << mipCoordinate_ << id << "\n" << std::flush;
        zi::shared_ptr< TriStripCollector > mesh;

        {
            zi::rwmutex::read_guard g( lock_ );
            if ( meshes_.count( id ) )
            {
                mesh = meshes_[ id ];
            }
            else
            {
                return;
            }
        }

        save_( id, mesh );
    }

};

#endif
