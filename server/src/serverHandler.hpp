#pragma once

#include "handler/handler.h"
#include "common/common.h"
#include "volume/volume.h"
#include "threads/taskManager.hpp"
#include "threads/taskManagerTypes.h"
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
                      public facebook::fb303::FacebookBase {
 private:
  fb_status::type status_;
  ServiceTracker serviceTracker_;

  std::string mesherHost_;
  int32_t mesherPort_;
  threads::threadPool threadPool_;

 public:
  serverHandler(std::string mesherHost, int32_t mesherPort)
      : FacebookBase("omni.server"),
        status_(fb_status::STARTING),
        serviceTracker_(this),
        mesherHost_(mesherHost),
        mesherPort_(mesherPort) {
    status_ = fb_status::ALIVE;
    threadPool_.start();
  }

  ~serverHandler() { threadPool_.stop(); }

  void get_tiles(std::vector<tile>& _return, const metadata& vol,
                 const vector3i& chunk, const viewType::type view,
                 const int32_t mipLevel, const rangei& depths) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_tiles", "get_tiles");
    handler::get_tiles(_return, vol,
                       coords::chunk(mipLevel, chunk.x, chunk.y, chunk.z),
                       common::Convert(view), depths.from, depths.to);
  }

  void get_seg_list_data(std::map<int, segData>& _return, const metadata& meta,
                         const std::set<int32_t>& segIds) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_seg_list_data",
                                "get_seg_list_data");
    handler::get_seg_list_data(_return, meta, segIds);
  }

  bool modify_global_mesh_data(const metadata& vol,
                               const std::set<int32_t>& addedSegIds,
                               const std::set<int32_t>& deletedSegIds,
                               int32_t segId) {
    ServiceMethod serviceMethod(&serviceTracker_, "modify_global_mesh_data",
                                "modify_global_mesh_data");

    MesherPtr mesher = makeMesher();
    if (!mesher) {
      return false;
    }

    std::set<uint32_t> addedIDs;
    std::set<uint32_t> modifiedIDs;
    FOR_EACH(id, addedSegIds) {
      addedIDs.insert(*id);
      modifiedIDs.insert(*id);
    }

    FOR_EACH(id, deletedSegIds) { modifiedIDs.insert(*id); }

    return handler::modify_global_mesh_data(mesher.get(), vol, addedIDs,
                                            modifiedIDs, segId);
  }

  void get_mesh(std::string& _return, const metadata& vol,
                const vector3i& chunk, int32_t mipLevel, int32_t segId) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_mesh", "get_mesh");
    handler::get_mesh(_return, vol, chunk, mipLevel, segId);
  }

  void get_obj(std::string& _return, const metadata& vol, const vector3i& chunk,
               int32_t mipLevel, int32_t segId) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_obj", "get_obj");
    handler::get_obj(_return, vol, chunk, mipLevel, segId);
  }

  void get_seeds(std::vector<std::map<int32_t, int32_t> >& _return,
                 const metadata& taskVolume, const std::set<int32_t>& selected,
                 const metadata& adjacentVolume) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_seeds", "get_seeds");
    handler::get_seeds(_return, taskVolume, selected, adjacentVolume);
  }

  // FB status monitor stuff
  fb_status::type getStatus() { return status_; }

  void getVersion(std::string& _return) { _return = "1"; }

  void setThreadManager(boost::shared_ptr<
      apache::thrift::concurrency::ThreadManager> threadManager) {
    serviceTracker_.setThreadManager(threadManager);
  }
  //private:
  typedef boost::shared_ptr<zi::mesh::RealTimeMesherClient> MesherPtr;
  MesherPtr makeMesher() {
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
      if (!transport->isOpen()) {
        std::cout << "WTF!!!!" << std::endl;
      }
    }
    catch (apache::thrift::TException & tx) {
      throw(tx);
    }

    return mesher;
  }
};

}
}
