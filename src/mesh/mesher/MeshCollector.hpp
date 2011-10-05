#pragma once

#include "TriStripCollector.hpp"
#include "mesh/io/v2/omMeshWriterV2.hpp"
#include "mesh/omMesh.h"
#include "mesh/omMeshCoord.h"

#include <zi/bits/cstdint.hpp>
#include <zi/bits/unordered_map.hpp>
#include <zi/bits/shared_ptr.hpp>
#include <zi/vl/vec.hpp>
#include <zi/concurrency/rwmutex.hpp>

#include <vector>

class MeshCollector
{
private:
    const om::chunkCoord coord_ ;
    OmMeshWriterV2 *const meshIO_;

    zi::spinlock lock_;

    typedef zi::unordered_map< segId, TriStripCollector* > map_t;
    map_t meshes_;

public:
    MeshCollector( const om::chunkCoord& coord,
                           OmMeshWriterV2* meshIO )
        : coord_( coord ),
          meshIO_( meshIO ),
          lock_(),
          meshes_()
    {}

    ~MeshCollector()
    {
        FOR_EACH(iter, meshes_){
            delete iter->second;
        }
    }

    void registerMeshPart( const segId segID )
    {
        TriStripCollector* tsc = NULL;

        {
            zi::guard g( lock_ );

            if ( 0 == meshes_.count( segID ) )
            {
                tsc = meshes_[ segID ] = new TriStripCollector();

            } else {
                tsc = meshes_[ segID ];
            }
        }

        tsc->registerPart();
    }

    TriStripCollector* getMesh( const segId segID )
    {
        zi::guard g( lock_ );

        if ( meshes_.count( segID ) == 0 )
        {
            return NULL;
        }

        return meshes_[ segID ];
    }

    void save( const segId segID )
    {
        TriStripCollector* mesh = getMesh( segID );

        if(!mesh)
        {
            std::cout << "skipping save for segID " << segID
                      << " in coord " << coord_ << "\n";
            return;
        }

        meshIO_->Save(segID, coord_, mesh,
                      om::BUFFER_WRITES, om::OVERWRITE);
    }
};
