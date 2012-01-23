#pragma once

#include <zi/vl/vec.hpp>
#include <zi/system.hpp>
#include <zi/bits/cstdint.hpp>
#include <zi/utility/static_assert.hpp>
#include <zi/mesh/quadratic_simplifier.hpp>
#include <zi/mesh/tri_mesh.hpp>
#include <zi/mesh/tri_stripper.hpp>
#include <zi/mesh/marching_cubes.hpp>
#include <zi/shared_ptr.hpp>
#include <zi/utility/container_utilities.hpp>
#include <zi/concurrency.hpp>

#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

#include "common/common.h"
#include "datalayer/memMappedFile.hpp"


namespace om {
namespace mesh {

class ziMesher {
private:
    datalayer::memMappedFile<uint32_t> data_file_    ;
    coords::volumeSystem               volume_system_;

    std::map<coords::chunk, std::vector<MeshCollector*> >  occurances_;
    std::map<coords::chunk, MeshCollector*>                chunkCollectors_;
    zi::task_manager::simple                               manager_;

    const int    numParallelChunks_ ;
    const int    numThreadsPerChunk_;
    const double downScallingFactor_;

public:
    ziMesher(const datalayer::memMappedFile<uint32_t>& data_file,
             const coords::volumeSystem& volume_system )
        : data_file_(data_file)
        , volume_system_(volume_system)
        , occurances_()
        , chunkCollectors_()
        , manager_( numParallelChunks() )
        , numParallelChunks_(numberParallelChunks())
        , numThreadsPerChunk_(zi::system::cpu_count / 2)
        , downScallingFactor_(OmMeshParams::GetDownScallingFactor())
    {
        printf("ziMesher: will process %d chunks at a time\n", numParallelChunks_);
        printf("ziMesher: will use %d threads per chunk\n", numThreadsPerChunk_);
    }

    ~ziMesher()
    {
        FOR_EACH(iter, chunkCollectors_)
        {
            delete iter->second;
        }
    }

    // add a chunk to the queue to be meshed
    // has to be called for all the chunks in the volume
    void enqueueChunk( coords::chunk coord, int max_mip_level )
    {
        // get the data to find unique values!
        std::set<uint32_t> unique_values;

        uint32_t* data = data_file_.GetPtrWithOffset(
            coord.chunkPtrOffset( &volume_system_, sizeof(uint32_t)) );

        for ( std::size_t i = 0; i < 128*128*128; ++i )
        {
            unique_values.insert(data[i]);
        }

        // for each mip level, register all IDs with the


    }


    void MeshFullVolume()
    {
        init();

        meshWriter_->Join();
        meshWriter_->CheckEverythingWasMeshed();

        meshManager_->Metadata()->SetMeshedAndStorageAsChunkFiles();
    }

    // void RemeshFullVolume()
    // {
    // if(redownsample){
    //     segmentation_->VolData()->downsample(segmentation_);
    // }
    //     OmChunkUtils::RefindUniqueChunkValues(segmentation_->GetID());
    // }

    om::shared_ptr<om::gui::progress> Progress(){
        return progress_.Progress();
    }

    void Progress(om::shared_ptr<om::gui::progress> p){
        progress_.Progress(p);
    }

private:
    void init()
    {
        om::shared_ptr<std::deque<OmChunkCoord> > levelZeroChunks =
            segmentation_->GetMipChunkCoords(0);

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

    void addValuesFromChunkAndDownsampledChunks(const OmChunkCoord& mip0coord)
    {
        const ChunkUniqueValues segIDs =
            segmentation_->ChunkUniqueValues()->Values(mip0coord, threshold_);

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

    void downsampleSegThroughAllMipLevels(const OmChunkCoord& mip0coord,
                                          const ChunkUniqueValues& segIDsMip0)
    {
        OmChunkCoord c = mip0coord.ParentCoord();

        // corner case: no MIP levels >0
        while (c.getLevel() <= rootMipLevel_)
        {
            registerSegIDs(mip0coord, c, segIDsMip0);

            c = c.ParentCoord();
        }
    }

    void downsampleSegThroughViewableMipLevels(const OmChunkCoord& mip0coord,
                                               const ChunkUniqueValues& segIDsMip0)
    {
        OmChunkCoord c = mip0coord.ParentCoord();

        // corner case: no MIP levels >0
        while (c.getLevel() <= rootMipLevel_)
        {
            std::deque<OmSegID> commonIDs;

            const ChunkUniqueValues segIDs =
                segmentation_->ChunkUniqueValues()->Values(c, threshold_);

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
    void registerSegIDs(const OmChunkCoord& mip0coord, const OmChunkCoord& c,
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

public:
    static void processSingleSegmentOnServer( double downscaling_factor,
                                              double max_err,
                                              double max_error_factor,
                                              zi::shared_ptr< zi::mesh::simplifier< double > > simplifier,
                                              std::vector<omni_server::tri_mesh>* targets )
    {
        simplifier->prepare();

        for ( std::size_t i = 0; i < targets->size(); ++i )
        {
            simplifier->optimize( simplifier->face_count() / downscaling_factor,
                                  max_err );

            std::vector< zi::vl::vec3d > pts ;
            std::vector< zi::vl::vec3d > nrms;
            std::vector< uint32_t > vtices ;
            std::vector< uint32_t > starts ;
            std::vector< uint32_t > lengths;

            simplifier->stripify( pts, nrms, vtices, starts, lengths );

            omni_server::tri_mesh& target = targets->operator[](i);

            target.verties.resize(vtices.size());
            std::copy(vtices.begin(), vtices.end(), target.verties.begin());
            //zi::containers::clear(vtices);

            target.starts.resize(starts.size());
            std::copy(starts.begin(), starts.end(), target.starts.begin());
            //zi::containers::clear(starts);

            target.lengths.resize(lengths.size());
            std::copy(lengths.begin(), lengths.end(), target.lengths.begin());
            //zi::containers::clear(lengths);

            target.pts.resize(pts.size());
            for ( std::size_t j = 0; j < pts.size(); ++j )
            {
                target.pts[j].x = pts[j].at(0);
                target.pts[j].y = pts[j].at(1);
                target.pts[j].z = pts[j].at(2);
            }
            //zi::containers::clear(pts);

            target.nrms.resize(nrms.size());
            for ( std::size_t j = 0; j < nrms.size(); ++j )
            {
                target.nrms[j].x = nrms[j].at(0);
                target.nrms[j].y = nrms[j].at(1);
                target.nrms[j].z = nrms[j].at(2);
            }
            //zi::containers::clear(nrms);

            max_err *= max_error_factor;
        }

        simplifier.reset();
    }

    static void mesh( std::vector<omni_server::simplified_tri_mesh> & _return,
                      const std::string& data,
                      const int32_t levels,
                      const double downscaling_factor,
                      const double max_error,
                      const double max_error_factor,
                      const omni_server::vector3d& scale)
    {

        std::cout << "Meshing of a chunk request recieved" << std::endl;

        zi::mesh::marching_cubes< int > cube_marcher;
        cube_marcher.marche( reinterpret_cast< const int* >(data.data()), 129, 129, 129 );

        zi::task_manager::simple manager( 16 );
        manager.start();

        _return.resize(cube_marcher.meshes().size());

        std::size_t idx = 0;

        FOR_EACH( it, cube_marcher.meshes() )
        {
            _return[idx].id = it->first;
            _return[idx].meshes.resize(levels);
            ++idx;
        }

        idx = 0;
        FOR_EACH( it, cube_marcher.meshes() )
        {
            if ( it->first != 0 )
            {
                zi::shared_ptr< zi::mesh::simplifier< double > >
                    spfy( new zi::mesh::simplifier< double >( 0 ) );

                cube_marcher.fill_simplifier< double >( *spfy, it->first,
                                                        0, 0, 0,
                                                        scale.z,
                                                        scale.y,
                                                        scale.x );

                // std::cout << "Meshing ID: " << _return[idx].id << " With "
                //           << _return[idx].meshes.size() << " levels" << std::endl;


                manager.push_back(
                    zi::bind( &ziMesher::processSingleSegmentOnServer,
                              downscaling_factor,
                              max_error,
                              max_error_factor,
                              spfy,
                              &_return[idx].meshes
                        ));
            }
            ++idx;
        }

        cube_marcher.clear();
        manager.join();
    }

private:
    void setupMarchingCube(zi::mesh::marching_cubes< int >& cube_marcher,
                           OmSegChunk* chunk)
    {
        OmImage< uint32_t, 3 > chunkData =
            OmChunkUtils::GetMeshOmImageData(segmentation_, chunk);

        OmChunkUtils::RewriteChunkAtThreshold(segmentation_, chunkData, threshold_);

        const OmSegID* chunkDataRaw = static_cast< const OmSegID* >( chunkData.getScalarPtr() );

        // remote cube_marcher call for the chunk
        // should return a remote reference so that we can refer to it later
        // and delete it at the end

        //std::string dataString( reinterpret_cast< const char* >(chunkDataRaw),
        //129*129*129*(sizeof(int)) );

        cube_marcher.marche( reinterpret_cast< const int* >(chunkDataRaw), 129, 129, 129 );
    }

    const int* getChunkDataRaw(OmSegChunk* chunk)
    {
        OmImage< uint32_t, 3 > chunkData =
            OmChunkUtils::GetMeshOmImageData(segmentation_, chunk);

        OmChunkUtils::RewriteChunkAtThreshold(segmentation_, chunkData, threshold_);

        const OmSegID* chunkDataRaw = static_cast< const OmSegID* >( chunkData.getScalarPtr() );

        return reinterpret_cast< const int* >(chunkDataRaw);
    }

    void processChunk( OmChunkCoord coord )
    {
        static const int chunkDim = segmentation_->Coords().GetChunkDimension();

        OmSegChunk* chunk = segmentation_->GetChunk(coord);

        const NormBbox& dstBbox = chunk->Mipping().GetNormExtent();

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
            segmentation_->ChunkUniqueValues()->Values(coord, threshold_);


        //zi::mesh::marching_cubes< int > cube_marcher;
        //setupMarchingCube(cube_marcher, chunk);

        //zi::task_manager::simple manager( numThreadsPerChunk_ );
        //manager.start();

        std::vector<omni_server::simplified_tri_mesh> results;

        omni_server::vector3d vscale;
        vscale.x = scale.at(0);
        vscale.y = scale.at(1);
        vscale.z = scale.at(2);


        // This is local version
        // -------------------
        // ziMesher::mesh( results, data_string, occurances_.begin()->second.size(),
        // downScallingFactor_, 90.0, 16, vscale );

        // This is remote version
        // -------------------

        if ( segIDs.size() > 0 )
        {
            OmImage< uint32_t, 3 > chunkData =
                OmChunkUtils::GetMeshOmImageData(segmentation_, chunk);

            OmChunkUtils::RewriteChunkAtThreshold(segmentation_, chunkData, threshold_);

            const OmSegID* chunkDataRaw = static_cast< const OmSegID* >( chunkData.getScalarPtr() );

            std::string data_string( reinterpret_cast< const char* >(chunkDataRaw),
                                     129*129*129*sizeof(int) );


            while (1)
            {
                using namespace apache::thrift;
                using namespace apache::thrift::protocol;
                using namespace apache::thrift::transport;

                boost::shared_ptr<TTransport> socket;

                std::string mesh_on = "";

                {
                    // connect to the scheduler
                    boost::shared_ptr<TSocket>           socket( new TSocket("192.168.1.74",8888) );
                    socket->setConnTimeout(200);
                    boost::shared_ptr<TTransport>        transport(new TBufferedTransport(socket));
                    boost::shared_ptr<TProtocol>         protocol(new TBinaryProtocol(transport));
                    omni_server::meshing_schedulerClient client(protocol);

                    try
                    {
                        transport->open();
                        client.suggest_host(mesh_on);
                    }
                    catch (...)
                    {
                        ::sleep(5);
                        continue;
                    }
                }

                if ( mesh_on == "" )
                {
                    ::sleep(5);
                    continue;
                }


                socket = boost::shared_ptr<TSocket>(new TSocket(mesh_on, 9999));

                boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
                boost::shared_ptr<TProtocol>  protocol(new TBinaryProtocol(transport));
                omni_server::mesherClient client(protocol);

                try
                {
                    transport->open();
                    client.mesh( results, data_string, occurances_.begin()->second.size(),
                                 downScallingFactor_, 90.0, 16, vscale );
                    break;
                }
                catch (...)
                {
                    ::sleep(5);
                    continue;
                }
            }


            {
                std::string tmp;
                tmp.swap(data_string);
            }

        }
        else
        {
            std::cout << "\rSkipping Chunk " << coord << " b/c there's nothing in there";
        }


        FOR_EACH( it, results )
        {
            const OmSegID segID = it->id;

            if (segIDs.contains(segID))
            {
                // std::cout << "Meshing Result: " << it->id << " With "
                //           << it->meshes.size() << " levels" << std::endl;

                std::vector< MeshCollector* >* targets = &occurances_[coord];

                ZI_ASSERT( it->meshes.size() == targets->size() );

                for ( std::size_t idx = 0; idx < targets->size(); ++idx )
                {

                    omni_server::tri_mesh& the_mesh = it->meshes[idx];

                    std::vector< zi::vl::vec3d > pts ;
                    std::vector< zi::vl::vec3d > nrms;
                    std::vector< uint32_t > vtices ;
                    std::vector< uint32_t > starts ;
                    std::vector< uint32_t > lengths;

                    vtices.resize(the_mesh.verties.size());
                    std::copy(the_mesh.verties.begin(),
                              the_mesh.verties.end(), vtices.begin() );
                    //zi::containers::clear(the_mesh.verties);

                    starts.resize(the_mesh.verties.size());
                    std::copy(the_mesh.starts.begin(),
                              the_mesh.starts.end(), starts.begin() );
                    //zi::containers::clear(the_mesh.starts);

                    lengths.resize(the_mesh.verties.size());
                    std::copy(the_mesh.lengths.begin(),
                              the_mesh.lengths.end(), lengths.begin() );
                    //zi::containers::clear(the_mesh.lengths);

                    pts.resize(the_mesh.pts.size());
                    for ( std::size_t j = 0; j < pts.size(); ++j )
                    {
                        pts[j].at(0) = the_mesh.pts[j].x;
                        pts[j].at(1) = the_mesh.pts[j].y;
                        pts[j].at(2) = the_mesh.pts[j].z;
                    }
                    //zi::containers::clear(the_mesh.pts);

                    nrms.resize(the_mesh.nrms.size());
                    for ( std::size_t j = 0; j < nrms.size(); ++j )
                    {
                        nrms[j].at(0) = the_mesh.nrms[j].x;
                        nrms[j].at(1) = the_mesh.nrms[j].y;
                        nrms[j].at(2) = the_mesh.nrms[j].z;
                    }
                    //zi::containers::clear(the_mesh.nrms);

                    // std::cout << "\t" << pts.size() << "\n\t"
                    //           << nrms.size() << "\n\t"
                    //           << vtices.size() << "\n\t"
                    //           << starts.size() << "\n\t"
                    //           << lengths.size() << std::endl;


                    TriStripCollector* tsc = targets->operator[](idx)->getMesh( segID );

                    if(tsc && tsc->append( pts,
                                           nrms,
                                           vtices,
                                           starts,
                                           lengths,
                                           maxScale,
                                           translate ) == 0 )
                    {
                        targets->operator[](idx)->save( segID );
                    }
                }
            }
        }


                // zi::shared_ptr< zi::mesh::simplifier< double > >
                //     spfy( new zi::mesh::simplifier< double >( 0 ) );

                // cube_marcher.fill_simplifier< double >( *spfy, segID,
                //                                         0, 0, 0,
                //                                         scale.at( 2 ),
                //                                         scale.at( 1 ),
                //                                         scale.at( 0 ) );

                // manager.push_back(
                //     zi::run_fn(
                //         zi::bind( &ziMesher::processSingleSegment, this,
                //                   segID,
                //                   maxScale,
                //                   translate,
                //                   spfy,
                //                   &occurances_[ coord ]
                //             )));
        //}
        //}

        //cube_marcher.clear();
        //manager.join();

        progress_.ChunkCompleted(coord);
    }

public:
    static int numberParallelChunks()
    {
        // each thread eats a lot of memory (pre loads the data)
        const int megsMemNeededPerChunk = 7500;

        const int sysMemMegs = zi::system::memory::total_mb();

        const int numChunks = sysMemMegs / megsMemNeededPerChunk;

        if(numChunks < 2){
            return 2;
        }

        return numChunks;
    }
};


} // namespace mesh
} // namespace om
