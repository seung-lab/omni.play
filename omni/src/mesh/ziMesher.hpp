#ifndef MESH_ZIMESHER_H_
#define MESH_ZIMESHER_H_

#include "common/omCommon.h"
#include "detail/MipChunkMeshCollector.hpp"
#include "detail/TriStripCollector.hpp"
#include "mesh/omMeshParams.hpp"
#include "project/omProject.h"
#include "utility/omLockedPODs.hpp"
#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omMipVolume.h"
#include "volume/omSegmentation.h"

#include <zi/vl/vec.hpp>
#include <zi/system.hpp>
#include <zi/bits/cstdint.hpp>
#include <zi/mesh/quadratic_simplifier.hpp>
#include <zi/mesh/tri_mesh.hpp>
#include <zi/mesh/tri_stripper.hpp>
#include <zi/mesh/marching_cubes.hpp>
#include <zi/shared_ptr.hpp>

#include "zi/omThreads.h"

class OmMipMeshManager;

class ziMesher
{
public:
    ziMesher( const OmID &segId, OmMipMeshManager *mmManager, int rootLevel )
        : segmentationId_(segId),
          mipMeshManager_(mmManager),
          rootMipLevel_(rootLevel),
          levelZeroChunks_(),
          chunkCollectors_()
    {
    }

    void addChunkCoord( const OmMipChunkCoord &c )
    {
        levelZeroChunks_.push_back(c);
    }

	void mesh(){
		init();
	}

    LockedInt64 numOfChunksToProcess;

private:

    OmID                           segmentationId_ ;
    OmMipMeshManager*              mipMeshManager_ ;
    int                            rootMipLevel_   ;
    std::vector< OmMipChunkCoord > levelZeroChunks_;

    std::map< OmMipChunkCoord, std::vector< zi::shared_ptr< MipChunkMeshCollector > > > occurances_;
    std::map< OmMipChunkCoord, zi::shared_ptr< MipChunkMeshCollector > > chunkCollectors_;

    void init()
    {
        numOfChunksToProcess.set(levelZeroChunks_.size());

        FOR_EACH( it, levelZeroChunks_ )
        {
            OmMipChunkCoord c = *it;

            OmMipChunkPtr chunk;
            OmProject::GetSegmentation( segmentationId_ ).GetChunk( chunk, *it );
            const OmSegIDsSet& idSet = chunk->GetDirectDataValues();

            chunkCollectors_.insert( std::make_pair( c, zi::shared_ptr< MipChunkMeshCollector >
                                                     ( new MipChunkMeshCollector
                                                       ( segmentationId_, c, mipMeshManager_ ) )));

            occurances_[ *it ].push_back( chunkCollectors_[ *it ] );

            FOR_EACH( cid, idSet )
            {
                chunkCollectors_[ c ]->registerMeshPart( *cid );
            }


            do
            {
                c = c.ParentCoord();

                if ( chunkCollectors_.count( c ) == 0 )
                {
                    chunkCollectors_.insert( std::make_pair
                                             ( c, zi::shared_ptr< MipChunkMeshCollector >
                                               ( new MipChunkMeshCollector
                                                 ( segmentationId_, c, mipMeshManager_ ) )));
                }

                FOR_EACH( cid, idSet )
                {
                    chunkCollectors_[ c ]->registerMeshPart( *cid );
                }

                occurances_[ *it ].push_back( chunkCollectors_[ c ] );


            } while (c.getLevel() < rootMipLevel_);

        }

        zi::task_manager::simple manager( 3 );
        manager.start();

        FOR_EACH( it, levelZeroChunks_ )
        {
            manager.push_back( zi::run_fn( zi::bind( &ziMesher::processChunk, this, *it ) ));
        }

        manager.join();

    }

    int getQueueSize()
    {
        //TODO: retrieve from omLocalPreferences? (purcaro)
        //Update by aleks: this actually shouldn't be too big, since
        // each thread eats a lot of memory (pre loads the data)
        int idealNum = zi::system::cpu_count;
        printf("ziMesher: will use %d threads\n", idealNum);
        return idealNum;
    }

public:

    void processSingleSegment( int id, double scale, zi::vl::vec3d trans,
                               zi::shared_ptr< zi::mesh::simplifier< double > > simplifier,
                               std::vector< zi::shared_ptr< MipChunkMeshCollector > > targets )
    {
        double max_err = 90.0;

        simplifier->prepare();

        FOR_EACH( it, targets )
        {
            simplifier->optimize( simplifier->face_count() / OmMeshParams::GetDownScallingFactor(),
								  max_err );

            std::vector< zi::vl::vec3d > pts ;
            std::vector< zi::vl::vec3d > nrms;
            std::vector< uint32_t > vtices ;
            std::vector< uint32_t > starts ;
            std::vector< uint32_t > lengths;

            simplifier->stripify( pts, nrms, vtices, starts, lengths );

            if ( (*it)->getMesh( id )->append( pts, nrms, vtices,
                                               starts, lengths, scale, trans ) == 0 )
            {
                //std::cout << "SHOULD SAVE: " << id << "\n";
                (*it)->save( id );
            }

            max_err *= 16;
        }

        simplifier.reset();
    }

    void processChunk( OmMipChunkCoord coor )
    {
        zi::task_manager::simple manager( getQueueSize() );

        OmMipChunkPtr chunk;
        OmProject::GetSegmentation( segmentationId_ ).GetChunk( chunk, coor );

        AxisAlignedBoundingBox< int >   srcBbox = chunk->GetExtent();
        AxisAlignedBoundingBox< float > dstBbox = chunk->GetNormExtent();

        Vector3f srcDim = srcBbox.getMax() - srcBbox.getMin();
        srcDim += Vector3f::ONE;

        Vector3f dstDim = dstBbox.getMax() - dstBbox.getMin();

        zi::vl::vec3d scale( dstDim.x / srcDim.x, dstDim.y / srcDim.y, dstDim.z / srcDim.z );

        dstDim = dstBbox.getMin();
        zi::vl::vec3d translate( dstDim.z, dstDim.y, dstDim.x );

        OmImage< uint32_t, 3 > image = chunk->GetMeshOmImageData();

        const OmSegID *image_data = static_cast< const OmSegID* >( image.getScalarPtr() );

        double maxScale = scale.max();
        scale /= maxScale;
        scale *= 0.5;

        zi::mesh::marching_cubes< int > cube_marcher;
        cube_marcher.marche( reinterpret_cast< const int* >( image_data ), 129, 129, 129 );

        const OmSegIDsSet& idSet = chunk->GetDirectDataValues();

        manager.start();

        FOR_EACH( it, cube_marcher.meshes() )
        {

            int segm_id = it->first;

            if ( idSet.find( segm_id ) != idSet.end() )
            {

                zi::shared_ptr< zi::mesh::simplifier< double > >
                    spfy( new zi::mesh::simplifier< double >( 0 ) );

                cube_marcher.fill_simplifier< double >( *spfy, segm_id,
                                                        0, 0, 0,
                                                        scale.at( 2 ), scale.at( 1 ), scale.at( 0 ) );

                manager.push_back( zi::run_fn( zi::bind( &ziMesher::processSingleSegment, this,
                                                         segm_id, maxScale, translate,
                                                         spfy, occurances_[ coor ] ) ));
            }
        }

        cube_marcher.clear();
        manager.join();

        numOfChunksToProcess.sub(1);
        std::cout << "finished chunk: " << coor << "; "
                  << numOfChunksToProcess.get() << " chunks left\n";
    }

};

#endif
