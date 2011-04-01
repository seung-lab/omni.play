#ifndef MESH_ZIMESHER_H_
#define MESH_ZIMESHER_H_

#include "common/omCommon.h"
#include "mesh/detail/MipChunkMeshCollector.hpp"
#include "mesh/detail/TriStripCollector.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "mesh/omMeshParams.hpp"
#include "utility/omLockedPODs.hpp"
#include "chunks/omSegChunk.h"
#include "chunks/omChunkCoord.h"
#include "volume/omSegmentation.h"
#include "chunks/omChunkUtils.hpp"

#include <zi/vl/vec.hpp>
#include <zi/system.hpp>
#include <zi/bits/cstdint.hpp>
#include <zi/mesh/quadratic_simplifier.hpp>
#include <zi/mesh/tri_mesh.hpp>
#include <zi/mesh/tri_stripper.hpp>
#include <zi/mesh/marching_cubes.hpp>
#include <zi/shared_ptr.hpp>

#include "zi/omThreads.h"

class ziMesher
{
private:
    static int numberParallelChunks()
    {
        const int megsMemNeededPerChunk = 5000;

        const int sysMemMegs =
            OmSystemInformation::get_total_system_memory_megs();

        int numChunks = sysMemMegs / megsMemNeededPerChunk;
        if(numChunks < 2){
            numChunks = 2;
        }

        printf("ziMesher: will process %d chunks at a time\n", numChunks);

        return numChunks;
    }

    static int getQueueSize()
    {
        //TODO: retrieve from omLocalPreferences? (purcaro)
        //Update by aleks: this actually shouldn't be too big, since
        // each thread eats a lot of memory (pre loads the data)
        const int idealNum = OmSystemInformation::get_num_cores() / 2;
        printf("ziMesher: will use %d threads\n", idealNum);
        return idealNum;
    }

public:
    ziMesher( OmSegmentation* segmentation, const double threshold )
        : segmentation_(segmentation)
        , rootMipLevel_(segmentation->Coords().GetRootMipLevel())
        , threshold_(threshold)
        , levelZeroChunks_()
        , chunkCollectors_()
        , meshWriter_(boost::make_shared<OmMeshWriterV2>(segmentation_, threshold_))
    {
    }

    ~ziMesher()
    {
    }

    void MeshFullVolume()
    {
        boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
            segmentation_->GetMipChunkCoords(0);

        levelZeroChunks_.assign(coordsPtr->begin(), coordsPtr->end());

        init();

        meshWriter_->Join();

        segmentation_->MeshManager(threshold_)->Metadata()->SetMeshedAndStorageAsChunkFiles();
    }

private:

    OmSegmentation *const       segmentation_;
    const int                   rootMipLevel_;
    const double                threshold_;
    std::vector< OmChunkCoord > levelZeroChunks_;

    std::map< OmChunkCoord, std::vector< zi::shared_ptr< MipChunkMeshCollector > > > occurances_;
    std::map< OmChunkCoord, zi::shared_ptr< MipChunkMeshCollector > > chunkCollectors_;

    boost::shared_ptr<OmMeshWriterV2> meshWriter_;

    LockedInt32 numOfChunksToProcess_;

    void init()
    {
        const size_t numChunks = levelZeroChunks_.size();
        size_t counter = 0;

        numOfChunksToProcess_.set(numChunks);

        FOR_EACH( it, levelZeroChunks_ )
        {
            ++counter;
            std::cout << "\rfinding values in chunk " << counter
                      << " of " << numChunks << "..."
                      << std::flush;

            OmTimer chunkTimer;

            addValuesFromChunkAndDownsampledChunks(*it);

            const double time = chunkTimer.s_elapsed();
            std::cout << " (done in " << time << " secs, "
                      << 1. / time << " chunks per sec)"
                      << std::flush;
        }

        printf("\n");

        segmentation_->ChunkUniqueValues()->Clear();

        zi::task_manager::simple manager( numberParallelChunks() );
        manager.start();

        FOR_EACH( it, levelZeroChunks_ )
        {
            manager.push_back( zi::run_fn( zi::bind( &ziMesher::processChunk, this, *it ) ));
        }

        manager.join();
    }

    void addValuesFromChunkAndDownsampledChunks(const OmChunkCoord& mip0coord)
    {
        const ChunkUniqueValues segIDs =
            segmentation_->ChunkUniqueValues()->Values(mip0coord, threshold_);

        chunkCollectors_.insert( std::make_pair( mip0coord, zi::shared_ptr< MipChunkMeshCollector >
                                                 ( new MipChunkMeshCollector
                                                   ( mip0coord, meshWriter_) )));

        occurances_[ mip0coord ].push_back( chunkCollectors_[ mip0coord ] );

        FOR_EACH( cid, segIDs )
        {
            chunkCollectors_[ mip0coord ]->registerMeshPart( *cid );
        }

        OmChunkCoord c = mip0coord.ParentCoord();

        while (c.getLevel() <= rootMipLevel_)
        {
            if ( chunkCollectors_.count( c ) == 0 )
            {
                chunkCollectors_.insert( std::make_pair
                                         ( c, zi::shared_ptr< MipChunkMeshCollector >
                                           ( new MipChunkMeshCollector
                                             ( c, meshWriter_ ) )));
            }

            FOR_EACH( cid, segIDs )
            {
                chunkCollectors_[ c ]->registerMeshPart( *cid );
            }

            occurances_[ mip0coord ].push_back( chunkCollectors_[ c ] );

            c = c.ParentCoord();
        }
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

    void processChunk( OmChunkCoord coor )
    {
        OmSegChunkPtr chunk;
        segmentation_->GetChunk( chunk, coor );

        AxisAlignedBoundingBox< int >   srcBbox = chunk->Mipping().GetExtent();
        AxisAlignedBoundingBox< float > dstBbox = chunk->Mipping().GetNormExtent();

        Vector3f srcDim = srcBbox.getMax() - srcBbox.getMin();
        srcDim += Vector3f::ONE;

        Vector3f dstDim = dstBbox.getMax() - dstBbox.getMin();

        zi::vl::vec3d scale( dstDim.x / srcDim.x, dstDim.y / srcDim.y, dstDim.z / srcDim.z );

        dstDim = dstBbox.getMin();
        zi::vl::vec3d translate( dstDim.z, dstDim.y, dstDim.x );

        OmImage< uint32_t, 3 > image =
            OmChunkUtils::GetMeshOmImageData(segmentation_, chunk);
        OmChunkUtils::RewriteChunkAtThreshold(segmentation_, image, threshold_);
        const OmSegID *image_data = static_cast< const OmSegID* >( image.getScalarPtr() );

        const ChunkUniqueValues segIDs =
            segmentation_->ChunkUniqueValues()->Values(coor, threshold_);

        double maxScale = scale.max();
        scale /= maxScale;
        scale *= 0.5;

        zi::mesh::marching_cubes< int > cube_marcher;
        cube_marcher.marche( reinterpret_cast< const int* >( image_data ), 129, 129, 129 );


        zi::task_manager::simple manager( getQueueSize() );
        manager.start();

        FOR_EACH( it, cube_marcher.meshes() )
        {
            int segm_id = it->first;

            if ( segIDs.find( segm_id ) != segIDs.end() )
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

        numOfChunksToProcess_.sub(1);

        std::cout << "finished chunk: " << coor << "; "
                  << numOfChunksToProcess_.get() << " chunks left\n";
    }

};

#endif
