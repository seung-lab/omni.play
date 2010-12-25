#ifndef OMNI_SRC_MESH_DETAIL_MIP_CHUNK_MESH_COLLECTOR_HPP
#define OMNI_SRC_MESH_DETAIL_MIP_CHUNK_MESH_COLLECTOR_HPP 1

#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshCoord.h"
#include "TriStripCollector.hpp"

#include "mesh/io/v2/omMeshWriterV2.hpp"

#include <zi/bits/cstdint.hpp>
#include <zi/bits/unordered_map.hpp>
#include <zi/bits/shared_ptr.hpp>
#include <zi/vl/vec.hpp>
#include <zi/concurrency/rwmutex.hpp>

#include <vector>

class MipChunkMeshCollector
{
private:
    OmMipChunkCoord    mipCoordinate_ ;
	boost::shared_ptr<OmMeshWriterV2> meshIO_;
    zi::rwmutex        lock_   ;

    zi::unordered_map< OmSegID, zi::shared_ptr< TriStripCollector > > meshes_;

public:
    MipChunkMeshCollector( const OmMipChunkCoord& coord,
						   boost::shared_ptr<OmMeshWriterV2> meshIO )
        : mipCoordinate_( coord ),
          meshIO_( meshIO ),
          lock_(),
          meshes_()
    {
    }

    void registerMeshPart( const OmSegID segID )
    {
        zi::rwmutex::write_guard g( lock_ );
        if ( meshes_.count( segID ) == 0 )
        {
            meshes_.insert( std::make_pair
                            ( segID, zi::shared_ptr< TriStripCollector >
                              ( new TriStripCollector )));
        }

        meshes_[ segID ]->registerPart();
    }

    zi::shared_ptr< TriStripCollector > getMesh( const OmSegID segID )
    {
        zi::rwmutex::read_guard g( lock_ );

        if ( meshes_.count( segID ) == 0 )
        {
            return zi::shared_ptr< TriStripCollector >();
        }

        return meshes_[ segID ];
    }

    void save( const OmSegID segID )
    {
        zi::shared_ptr< TriStripCollector > mesh;
        {
            zi::rwmutex::read_guard g( lock_ );
            if ( meshes_.count( segID ) )
            {
                mesh = meshes_[ segID ];
            }
            else
            {
                return;
            }
        }

		meshIO_->Save(segID, mipCoordinate_, mesh,
					  om::BUFFER_WRITES, om::OVERWRITE);
    }
};

#endif
