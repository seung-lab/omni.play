// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "server.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::om::server;

class serverHandler : virtual public serverIf {
 public:
  serverHandler() {
    // Your initialization goes here
  }

  void add_chunk(const metadata& vol, const vector3i& chunk, const std::string& data) {
    // Your implementation goes here
    printf("add_chunk\n");
  }

  void delete_chunk(const metadata& vol, const vector3i& chunk) {
    // Your implementation goes here
    printf("delete_chunk\n");
  }

  void get_chunk(std::string& _return, const metadata& vol, const vector3i& chunk) {
    // Your implementation goes here
    printf("get_chunk\n");
  }

  void get_graph(std::vector<edge> & _return, const metadata& vol) {
    // Your implementation goes here
    printf("get_graph\n");
  }

  void get_mst(std::vector<edge> & _return, const metadata& vol) {
    // Your implementation goes here
    printf("get_mst\n");
  }

  void create_segmentation(metadata& _return, const metadata& chan, const int32_t newVolId, const std::vector<std::string> & features) {
    // Your implementation goes here
    printf("create_segmentation\n");
  }

  void get_chan_tile(tile& _return, const metadata& vol, const vector3d& point, const viewType::type view) {
    // Your implementation goes here
    printf("get_chan_tile\n");
  }

  void get_seg_tiles(std::map<std::string, tile> & _return, const metadata& vol, const int32_t segId, const bbox& segBbox, const viewType::type view) {
    // Your implementation goes here
    printf("get_seg_tiles\n");
  }

  int32_t get_seg_id(const metadata& vol, const vector3d& point) {
    // Your implementation goes here
    printf("get_seg_id\n");
  }

  void get_seg_data(segData& _return, const metadata& vol, const int32_t segId) {
    // Your implementation goes here
    printf("get_seg_data\n");
  }

  void get_seg_list_data(std::map<int32_t, segData> & _return, const metadata& vol, const std::set<int32_t> & segIds) {
    // Your implementation goes here
    printf("get_seg_list_data\n");
  }

  void get_seg_ids(std::set<int32_t> & _return, const metadata& vol, const vector3d& point, const int32_t radius, const viewType::type view) {
    // Your implementation goes here
    printf("get_seg_ids\n");
  }

  void get_mesh(std::string& _return, const std::string& uri, const vector3i& chunk, const int32_t mipLevel, const int32_t segId) {
    // Your implementation goes here
    printf("get_mesh\n");
  }

  void get_remesh(std::string& _return, const std::string& uri, const vector3i& chunk, const int32_t mipLevel, const std::set<int32_t> & segIds) {
    // Your implementation goes here
    printf("get_remesh\n");
  }

  void get_obj(std::string& _return, const std::string& uri, const vector3i& chunk, const int32_t mipLevel, const int32_t segId) {
    // Your implementation goes here
    printf("get_obj\n");
  }

  double compare_results(const std::vector<result> & old_results, const result& new_result) {
    // Your implementation goes here
    printf("compare_results\n");
  }

  void get_seeds(std::vector<std::set<int32_t> > & _return, const metadata& taskVolume, const std::set<int32_t> & selected, const metadata& adjacentVolume) {
    // Your implementation goes here
    printf("get_seeds\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<serverHandler> handler(new serverHandler());
  shared_ptr<TProcessor> processor(new serverProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

