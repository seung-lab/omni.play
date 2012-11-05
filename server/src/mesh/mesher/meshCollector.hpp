#pragma once

#include "mesh/mesher/triStripCollector.hpp"

#include <zi/bits/cstdint.hpp>
#include <zi/bits/unordered_map.hpp>
#include <zi/bits/shared_ptr.hpp>
#include <zi/vl/vec.hpp>
#include <zi/concurrency/rwmutex.hpp>

#include <vector>

namespace om {
namespace mesh {

class MeshCollector
{
private:
    const coords::Chunk coord_ ;
    //OmMeshWriterV2 *const meshIO_;

    zi::spinlock lock_;

    typedef zi::unordered_map< common::segId, TriStripCollector* > map_t;
    map_t meshes_;

public:
    MeshCollector( const coords::Chunk& coord)//, OmMeshWriterV2* meshIO )
        : coord_( coord )
        //, meshIO_( meshIO )
        , lock_()
        , meshes_()
    {}

    ~MeshCollector()
    {
        FOR_EACH (iter, meshes_)
        {
            delete iter->second;
        }
    }

    void registerMeshPart( const common::segId segID )
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

    TriStripCollector* getMesh( const common::segId segID )
    {
        zi::guard g( lock_ );

        if ( meshes_.count( segID ) == 0 )
        {
            return NULL;
        }

        return meshes_[ segID ];
    }

    void save( const common::segId segID )
    {
        TriStripCollector* mesh = getMesh( segID );

        if(!mesh)
        {
            std::cout << "skipping save for segID " << segID
                      << " in coord " << coord_ << "\n";
            return;
        }

        //meshIO_->Save(segID, coord_, mesh,
         //             om::common::BUFFER_WRITES, om::common::OVERWRITE);
    }
};

}} // namespace om::mesh