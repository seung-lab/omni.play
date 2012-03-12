#pragma once

#include "chunks/chunkUtils.hpp"
#include "common/common.h"
#include "mesh/mesher/meshCollector.hpp"
#include "mesh/mesher/triStripCollector.hpp"

#include "mesh/meshParams.hpp"
#include "utility/lockedPODs.hpp"

#include <zi/vl/vec.hpp>
#include <zi/system.hpp>
#include <zi/bits/cstdint.hpp>
#include <zi/mesh/quadratic_simplifier.hpp>
#include <zi/mesh/tri_mesh.hpp>
#include <zi/mesh/tri_stripper.hpp>
#include <zi/mesh/marching_cubes.hpp>
#include <zi/shared_ptr.hpp>

#include "zi/threads.h"

namespace om {
namespace mesh {

class ziMesher {
private:
    const volume::volume& vol_;
    const int rootMipLevel_;

    std::map<coords::chunk, std::vector<MeshCollector*> > occurances_;
    std::map<coords::chunk, MeshCollector*> chunkCollectors_;

    const int numParallelChunks_;
    const int numThreadsPerChunk_;
    const double downScallingFactor_;

public:
    ziMesher(const volume::volume& vol, const double threshold)
        : vol_(vol)
        , rootMipLevel_(vol.CoordinateSystem()().GetRootMipLevel())
        , threshold_(threshold)
        , chunkCollectors_()
        , mesh::manager_(vol.MeshManager(threshold))
        , meshWriter_(new meshWriter(mesh::manager_))
        , numParallelChunks_(numberParallelChunks())
        , numThreadsPerChunk_(zi::system::cpu_count / 2)
        , downScallingFactor_(meshParams::GetDownScallingFactor())
    {
        printf("ziMesher: will process %d chunks at a time\n", numParallelChunks_);
        printf("ziMesher: will use %d threads per chunk\n", numThreadsPerChunk_);
    }

    ~ziMesher()
    {
        FOR_EACH(iter, chunkCollectors_){
            delete iter->second;
        }
    }

    void MeshFullVolume()
    {
        init();

        meshWriter_->Join();
        meshWriter_->CheckEverythingWasMeshed();

        mesh::manager_->Metadata()->SetMeshedAndStorageAsChunkFiles();
    }

    // void RemeshFullVolume()
    // {
    // if(redownsample){
    //     vol_.VolData()->downsample(vol_);
    // }
    //     chunkUtils::RefindUniqueChunkValues(vol_.GetID());
    // }
private:

    void init()
    {
        boost::shared_ptr<std::deque<coords::chunk> > levelZeroChunks =
            vol_.CoordSystem().GetMipChunkCoords(0);

        progress_.SetTotalNumChunks(levelZeroChunks->size());

        FOR_EACH( it, *levelZeroChunks ){
            addValuesFromChunkAndDownsampledChunks(*it);
        }

        std::cout << "\nstarting meshing...\n";

        zi::task_manager::simple manager( numParallelChunks_ );
        manager.start();

        FOR_EACH( it, *levelZeroChunks )
        {
            manager.push_back(
                zi::run_fn(
                    zi::bind( &ziMesher::processChunk, this, *it ) ));
        }

        manager.join();

        std::cout << "\ndone meshing...\n";
    }

    void addValuesFromChunkAndDownsampledChunks(const coords::chunk& mip0coord)
    {
        const ChunkUniqueValues segIDs =
            vol_.ChunkUniqueValues()->Values(mip0coord, threshold_);

        chunkCollectors_.insert(
            std::make_pair( mip0coord, new MeshCollector( mip0coord,
                                                          meshWriter_.get() )));

        occurances_[ mip0coord ].push_back( chunkCollectors_[ mip0coord ] );

        FOR_EACH( cid, segIDs )
        {
            chunkCollectors_[ mip0coord ]->registerMeshPart( *cid );
        }

        downsampleSegThroughAllMipLevels(mip0coord, segIDs);
        //downsampleSegThroughViewableMipLevels(mip0coord, segIDs);
    }

    void downsampleSegThroughAllMipLevels(const coords::chunk& mip0coord,
                                          const ChunkUniqueValues& segIDsMip0)
    {
        coords::chunk c = mip0coord.ParentCoord();

        // corner case: no MIP levels >0
        while (c.getLevel() <= rootMipLevel_)
        {
            registerSegIDs(mip0coord, c, segIDsMip0);

            c = c.ParentCoord();
        }
    }

    void downsampleSegThroughViewableMipLevels(const coords::chunk& mip0coord,
                                               const ChunkUniqueValues& segIDsMip0)
    {
        coords::chunk c = mip0coord.ParentCoord();

        corner case: no MIP levels >0
        while (c.getLevel() <= rootMipLevel_)
        {
            std::deque<common::segId> commonIDs;

            const ChunkUniqueValues segIDs =
                vol_.ChunkUniqueValues()->Values(c, threshold_);

            FOR_EACH( cid, segIDsMip0 )
            {
                if(segIDs.contains(*cid)){
                    commonIDs.push_back(*cid);
                }
            }

            if(commonIDs.empty()){
                break;
            }

            registerSegIDs(mip0coord, c, commonIDs);

            c = c.ParentCoord();
        }
    }

    template <typename C>
    void registerSegIDs(const coords::chunk& mip0coord, const coords::chunk& c,
                        const C& segIDs)
    {
        if ( chunkCollectors_.count( c ) == 0 )
        {
            chunkCollectors_.insert(
                std::make_pair( c, new MeshCollector( c,
                                                      meshWriter_.get() )));
        }

        FOR_EACH( cid, segIDs )
        {
            chunkCollectors_[ c ]->registerMeshPart( *cid );
        }

        occurances_[ mip0coord ].push_back( chunkCollectors_[ c ] );
    }

    void processSingleSegment( int id, double scale, zi::vl::vec3d trans,
                               zi::shared_ptr< zi::mesh::simplifier< double > > simplifier,
                               std::vector< MeshCollector* >* targets )
    {
        double max_err = 90.0;

        simplifier->prepare();

        FOR_EACH( it, *targets )
        {
            simplifier->optimize( simplifier->face_count() / downScallingFactor_,
                                  max_err );

            std::vector< zi::vl::vec3d > pts ;
            std::vector< zi::vl::vec3d > nrms;
            std::vector< uint32_t > vtices ;
            std::vector< uint32_t > starts ;
            std::vector< uint32_t > lengths;

            simplifier->stripify( pts, nrms, vtices, starts, lengths );

            TriStripCollector* tsc = (*it)->getMesh( id );

            if(tsc && tsc->append( pts, nrms, vtices,
                                   starts, lengths, scale, trans ) == 0 )
            {
                (*it)->save( id );
            }

            max_err *= 16;
        }

        simplifier.reset();
    }

    void setupMarchingCube(zi::mesh::marching_cubes< int >& cube_marcher,
                           segChunk* chunk)
    {
        OmImage< uint32_t, 3 > chunkData =
            chunkUtils::GetMeshOmImageData(vol_, chunk);

        chunkUtils::RewriteChunkAtThreshold(vol_, chunkData, threshold_);

        const common::segId* chunkDataRaw = static_cast< const common::segId* >( chunkData.getScalarPtr() );

        cube_marcher.marche( reinterpret_cast< const int* >(chunkDataRaw), 129, 129, 129 );
    }

    void processChunk( coords::chunk coord )
    {
        static const int chunkDim = vol_.CoordinateSystem()().GetChunkDimension();

        segChunk* chunk = vol_.GetChunk(coord);

        const coords::normBbox& dstBbox = chunk->Mipping().GetNormExtent();

        Vector3f dstDim = dstBbox.getDimensions();

        zi::vl::vec3d scale( dstDim.x / chunkDim,
                             dstDim.y / chunkDim,
                             dstDim.z / chunkDim );

        dstDim = dstBbox.getMin();

        const zi::vl::vec3d translate( dstDim.z, dstDim.y, dstDim.x );

        const double maxScale = scale.max();
        scale /= maxScale;
        scale *= 0.5;

        const ChunkUniqueValues segIDs =
            vol_.ChunkUniqueValues()->Values(coord, threshold_);

        zi::mesh::marching_cubes< int > cube_marcher;
        setupMarchingCube(cube_marcher, chunk);

        zi::task_manager::simple manager( numThreadsPerChunk_ );
        manager.start();

        FOR_EACH( it, cube_marcher.meshes() )
        {
            const common::segId segID = it->first;

            if(segIDs.contains(segID))
            {
                zi::shared_ptr< zi::mesh::simplifier< double > >
                    spfy( new zi::mesh::simplifier< double >( 0 ) );

                cube_marcher.fill_simplifier< double >( *spfy, segID,
                                                        0, 0, 0,
                                                        scale.at( 2 ),
                                                        scale.at( 1 ),
                                                        scale.at( 0 ) );

                manager.push_back(
                    zi::run_fn(
                        zi::bind( &ziMesher::processSingleSegment, this,
                                  segID,
                                  maxScale,
                                  translate,
                                  spfy,
                                  &occurances_[ coord ]
                            )));
            }
        }

        cube_marcher.clear();

        manager.join();

        progress_.ChunkCompleted(coord);
    }

    static int numberParallelChunks()
    {
        // each thread eats a lot of memory (pre loads the data)
        const int megsMemNeededPerChunk = 5000;

        const int sysMemMegs = zi::system::memory::total_mb();

        const int numChunks = sysMemMegs / megsMemNeededPerChunk;

        if(numChunks < 2){
            return 2;
        }

        return numChunks;
    }
};

}} // namespace om::mesh