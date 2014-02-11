#pragma once

#include "handler/handler.h"
#include "common/common.h"
#include "volume/volume.h"
#include "volume/segmentation.h"
#include "threads/taskManager.hpp"
#include "threads/taskManagerTypes.h"
#include "utility/ServiceTracker.h"
#include "utility/FacebookBase.h"
#include "utility/convert.hpp"
#include "RealTimeMesher.h"

#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>
#include <zi/singleton.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace facebook::fb303;

namespace om {
namespace server {

class MesherConnector : public zi::enable_singleton_of_this<MesherConnector> {
 public:
  typedef boost::shared_ptr<zi::mesh::RealTimeMesherIf> MesherPtr;

  inline static MesherPtr MakeMesher() { return instance().makeMesher(); }

  inline static void set_mesherHost(std::string host) {
    instance().mesherHost_ = host;
  }

  inline static void set_mesherPort(int32_t port) {
    instance().mesherPort_ = port;
  }

 private:
  MesherPtr makeMesher() {
    using namespace apache::thrift;
    using namespace apache::thrift::transport;
    using namespace apache::thrift::protocol;

    auto socket = boost::make_shared<TSocket>(mesherHost_, mesherPort_);
    auto transport = boost::make_shared<TFramedTransport>(socket);
    auto protocol = boost::make_shared<TBinaryProtocol>(transport);
    auto mesher = boost::make_shared<zi::mesh::RealTimeMesherClient>(protocol);

    try {
      transport->open();
      if (!transport->isOpen()) {
        log_debugs << "Unable to open transport.";
      }
    }
    catch (apache::thrift::TException& tx) {
      throw(tx);
    }

    return mesher;
  }

  std::string mesherHost_;
  int32_t mesherPort_;

  friend class zi::singleton<MesherConnector>;
};

class serverHandler : virtual public serverIf,
                      public facebook::fb303::FacebookBase {
 private:
  fb_status::type status_;
  ServiceTracker serviceTracker_;

  std::string mesherHost_;
  int32_t mesherPort_;
  thread::ThreadPool threadPool_;

 public:
  serverHandler(std::string mesherHost, int32_t mesherPort)
      : FacebookBase("omni.server"),
        status_(fb_status::STARTING),
        serviceTracker_(this, &serverHandler::logMethod),
        mesherHost_(mesherHost),
        mesherPort_(mesherPort) {
    status_ = fb_status::ALIVE;
    threadPool_.start(1);
  }

  ~serverHandler() { threadPool_.stop(); }

  void get_tiles(std::vector<tile>& _return, const metadata& meta,
                 const vector3i& chunk, const viewType::type view,
                 const int32_t mipLevel, const rangei& depths) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_tiles", "get_tiles");
    volume::Volume v(volumePath(meta));
    handler::get_tiles(_return, v,
                       coords::Chunk(mipLevel, chunk.x, chunk.y, chunk.z),
                       utility::Convert(view), depths.from, depths.to);
  }

  void get_seg_list_data(std::map<int, segData>& _return, const metadata& meta,
                         const std::set<int32_t>& segIds) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_seg_list_data",
                                "get_seg_list_data");
    if (!file::Paths::IsValid(meta.uri)) {
      throw ArgException(std::string("Invalid metadata uri: " + meta.uri));
    }

    file::Paths p(meta.uri);
    volume::Segmentation v(p, 1);
    handler::get_seg_list_data(_return, v, segIds);
  }

  bool modify_global_mesh_data(const metadata& meta,
                               const std::set<int32_t>& addedSegIds,
                               const std::set<int32_t>& deletedSegIds,
                               int32_t segId) {
    ServiceMethod serviceMethod(&serviceTracker_, "modify_global_mesh_data",
                                "modify_global_mesh_data");

    if (!file::Paths::IsValid(meta.uri)) {
      throw ArgException(std::string("Invalid metadata uri: " + meta.uri));
    }
    file::Paths p(meta.uri);

    std::set<uint32_t> addedIDs;
    std::set<uint32_t> modifiedIDs;
    addedIDs.insert(addedSegIds.begin(), addedSegIds.end());
    modifiedIDs.insert(addedSegIds.begin(), addedSegIds.end());
    modifiedIDs.insert(deletedSegIds.begin(), deletedSegIds.end());

    auto rtmConnector = std::bind(&serverHandler::makeMesher, this);

    threadPool_.push_back([p, rtmConnector, addedIDs, modifiedIDs, segId]() {
      volume::Segmentation v(p, 1);
      handler::modify_global_mesh_data(rtmConnector, v, addedIDs, modifiedIDs,
                                       segId);
    });
    return true;
  }

  void get_mesh(std::string& _return, const metadata& meta,
                const vector3i& chunk, int32_t mipLevel, int32_t segId) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_mesh", "get_mesh");
    if (!file::Paths::IsValid(meta.uri)) {
      throw ArgException(std::string("Invalid metadata uri: " + meta.uri));
    }
    file::Paths p(meta.uri);
    volume::Segmentation v(p, 1);
    handler::get_mesh(_return, v, chunk, mipLevel, segId);
  }

  void get_obj(std::string& _return, const metadata& meta,
               const vector3i& chunk, int32_t mipLevel, int32_t segId) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_obj", "get_obj");
    if (!file::Paths::IsValid(meta.uri)) {
      throw ArgException(std::string("Invalid metadata uri: " + meta.uri));
    }
    file::Paths p(meta.uri);
    volume::Segmentation v(p, 1);
    handler::get_obj(_return, v, chunk, mipLevel, segId);
  }

  void get_seeds(std::vector<std::map<int32_t, int32_t>>& _return,
                 const metadata& taskVolume, const std::set<int32_t>& selected,
                 const metadata& adjacentVolume) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_seeds", "get_seeds");

    if (!file::Paths::IsValid(taskVolume.uri)) {
      throw ArgException(
          std::string("Invalid metadata uri: " + taskVolume.uri));
    }
    if (!file::Paths::IsValid(adjacentVolume.uri)) {
      throw ArgException(
          std::string("Invalid metadata uri: " + adjacentVolume.uri));
    }

    file::Paths pathsTaskVolume(taskVolume.uri);
    volume::Segmentation task(pathsTaskVolume, 1);
    file::Paths pathsAdjacentVolume(adjacentVolume.uri);
    volume::Segmentation adj(pathsAdjacentVolume, 1);
    handler::get_seeds(_return, task, selected, adj);
  }

  void get_mst(std::vector<affinity>& _return, const metadata& meta) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_mst", "get_mst");
    if (!file::Paths::IsValid(meta.uri)) {
      throw ArgException(std::string("Invalid metadata uri: " + meta.uri));
    }
    file::Paths p(meta.uri);
    volume::Segmentation v(p, 1);
    handler::get_mst(_return, v);
  }

  void get_connected_groups(
      std::vector<group>& _return, const metadata& meta,
      const std::map<int32_t, std::set<int32_t>>& groups) {
    ServiceMethod serviceMethod(&serviceTracker_, "get_connected_groups",
                                "get_connected_groups");
    if (!file::Paths::IsValid(meta.uri)) {
      throw ArgException(std::string("Invalid metadata uri: " + meta.uri));
    }
    file::Paths p(meta.uri);
    volume::Segmentation v(p, 1);

    std::unordered_map<int, common::SegIDSet> gs;
    for (const auto& group : groups) {
      gs[group.first].insert(group.second.begin(), group.second.end());
    }
    handler::get_connected_groups(_return, v, gs);
  }

  // FB status monitor stuff
  fb_status::type getStatus() { return status_; }

  void getVersion(std::string& _return) { _return = "1"; }

  void setThreadManager(boost::shared_ptr<
      apache::thrift::concurrency::ThreadManager> threadManager) {
    serviceTracker_.setThreadManager(threadManager);
  }
  // private:
  typedef boost::shared_ptr<zi::mesh::RealTimeMesherIf> MesherPtr;
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

    boost::shared_ptr<zi::mesh::RealTimeMesherClient> mesher =
        boost::shared_ptr<zi::mesh::RealTimeMesherClient>(
            new zi::mesh::RealTimeMesherClient(protocol));

    try {
      transport->open();
      if (!transport->isOpen()) {

        log_errors << "Unable to open transport.";
      }
    }
    catch (apache::thrift::TException& tx) {
      throw(tx);
    }

    return mesher;
  }

 private:
  static void logMethod(int, const std::string& str) { log_infos << str; }

  std::string volumePath(const metadata& meta) {
    if (!file::Paths::IsValid(meta.uri)) {
      throw ArgException(std::string("Invalid metadata uri: " + meta.uri));
    }

    file::Paths p(meta.uri);
    if (meta.vol_type == volType::CHANNEL) {
      return p.Channel(1).string();
    } else {
      return p.Segmentation(1).string();
    }
  }
};
}
}  // om::server::
