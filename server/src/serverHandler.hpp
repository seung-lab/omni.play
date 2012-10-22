 #pragma once

#include "handler/handler.h"
#include "common/common.h"
#include "volume/volume.h"
#include "utility/ServiceTracker.h"
#include "utility/FacebookBase.h"
#include "RealTimeMesher.h"

#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

using namespace facebook::fb303;

namespace om {
namespace server {

class serverHandler : virtual public serverIf,
                      public facebook::fb303::FacebookBase
{
private:
    fb_status::type status_;
    ServiceTracker serviceTracker_;

    std::string mesherHost_;
    int32_t mesherPort_;

public:
    serverHandler(std::string mesherHost, int32_t mesherPort)
        : FacebookBase("omni.server")
        , status_(fb_status::STARTING)
        , serviceTracker_(this)
        , mesherHost_(mesherHost)
        , mesherPort_(mesherPort)
    {
        status_ = fb_status::ALIVE;
    }

    void add_chunk(const metadata& vol, const vector3i& chunk, const std::string& data) {
    }

    void delete_chunk(const metadata& vol, const vector3i& chunk){
    }

    void get_chunk(std::string& _return, const metadata& vol, const vector3i& chunk){
        handler::get_chunk(_return, vol, chunk);
    }

    void get_mst(std::vector<edge>& _return, const metadata& vol){
        handler::get_mst(_return, vol);
    }

    void get_graph(std::vector<edge>& _return, const metadata& vol){
        handler::get_graph(_return, vol);
    }

    void create_segmentation(metadata& _return,
                             const metadata& chan,
                             const int32_t newVolId,
                             const std::vector<std::string> & features) {

    }

    void get_chan_tile(server::tile& _return,
                       const server::metadata& vol,
                       const server::vector3d& point,
                       const server::viewType::type view)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_chan_tile", "get_chan_tile");
        handler::get_chan_tile(_return, vol, point, common::Convert(view));
    }

    void get_seg_tiles(std::map<std::string, tile> & _return,
                       const metadata& vol,
                       const int32_t segId,
                       const bbox& segBbox,
                       const server::viewType::type view)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_seg_tiles", "get_seg_tiles");
        handler::get_seg_tiles(_return, vol, segId, segBbox, common::Convert(view));
    }

    int32_t get_seg_id(const metadata& vol, const vector3d& point) {
        ServiceMethod serviceMethod(&serviceTracker_, "get_seg_id", "get_seg_id");
        return handler::get_seg_id(vol, point);
    }

    void get_seg_ids(std::set<int32_t> & _return,
                     const metadata& vol,
                     const vector3d& point,
                     const int32_t radius,
                     const viewType::type view)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_seg_ids", "get_seg_ids");
        handler::get_seg_ids(_return, vol, point, radius, common::Convert(view));
    }

    void get_seg_data(segData& _return, const metadata& meta, const int32_t segId)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_seg_data", "get_seg_data");
        handler::get_seg_data(_return, meta, segId);
    }

    void get_seg_list_data(std::map<int, segData>& _return,
                           const metadata& meta,
                           const std::set<int32_t>& segIds)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_seg_list_data", "get_seg_list_data");
        handler::get_seg_list_data(_return, meta, segIds);
    }

    double compare_results(const std::vector<result> & old_results, const result& new_result)
    {
        return handler::compare_results(old_results, new_result);
    }

    void get_mesh(std::string& _return,
                  const std::string& uri,
                  const vector3i& chunk,
                  int32_t mipLevel,
                  int32_t segId)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_mesh", "get_mesh");
        handler::get_mesh(_return, uri, chunk, mipLevel, segId);
    }

	void update_global_mesh(const metadata& vol,
	                        const std::set<int32_t>& segIds,
                            int32_t segId)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "update_global_mesh", "update_global_mesh");

        MesherPtr mesher = makeMesher();

        std::set<uint32_t> ids;
        FOR_EACH(id, segIds) {
        	ids.insert(*id);
        }
        handler::update_global_mesh(mesher.get(), vol, ids, segId);
    }


    void get_remesh(std::string& _return,
                    const std::string& uri,
                    const vector3i& chunk,
                    int32_t mipLevel,
                    const std::set<int32_t>& segIds)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_remesh", "get_remesh");
        handler::get_remesh(_return, uri, chunk, mipLevel, segIds);
    }

    void get_obj(std::string& _return,
                 const std::string& uri,
                 const vector3i& chunk,
                 int32_t mipLevel,
                 int32_t segId)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_obj", "get_obj");
        handler::get_obj(_return, uri, chunk, mipLevel, segId);
    }

    void get_seeds(std::vector<std::set<int32_t> >& _return,
                   const metadata& taskVolume,
                   const std::set<int32_t>& selected,
                   const metadata& adjacentVolume)
    {
        ServiceMethod serviceMethod(&serviceTracker_, "get_seeds", "get_seeds");
        handler::get_seeds(_return, taskVolume, selected, adjacentVolume);
    }

    fb_status::type getStatus() {
        return status_;
    }

    void getVersion(std::string& _return) { _return = "1"; }

    void setThreadManager(boost::shared_ptr<apache::thrift::concurrency::ThreadManager> threadManager) {
        serviceTracker_.setThreadManager(threadManager);
    }
//private:
	typedef boost::shared_ptr<zi::mesh::RealTimeMesherClient> MesherPtr;
	MesherPtr makeMesher()
	{
		using namespace apache::thrift;
		using namespace apache::thrift::transport;
		using namespace apache::thrift::protocol;

		boost::shared_ptr<TSocket> socket =
			boost::shared_ptr<TSocket>(new TSocket(mesherHost_, mesherPort_));

		boost::shared_ptr<TTransport> transport =
		    boost::shared_ptr<TTransport>(new TFramedTransport(socket));

		boost::shared_ptr<TProtocol> protocol =
		    boost::shared_ptr<TProtocol>(new TBinaryProtocol(transport));

		MesherPtr mesher = MesherPtr(new zi::mesh::RealTimeMesherClient(protocol));

		try {
		    transport->open();
		} catch (apache::thrift::TException &tx) {
		    throw(tx);
		}

		return mesher;
	}
};

}
}
