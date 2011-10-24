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

using namespace om::common;

class serverHandler : virtual public serverIf {
 public:
  serverHandler() {
    // Your initialization goes here
  }

  void get_volume_bounds(bbox& _return) {
    // Your implementation goes here
    printf("get_volume_bounds\n");
  }

  void get_chan_tile(tile& _return, const vector3d& point, const int32_t mipLevel) {
    // Your implementation goes here
    printf("get_chan_tile\n");
  }

  void get_seg_tile(tile& _return, const vector3d& point, const int32_t mipLevel, const int32_t segId) {
    // Your implementation goes here
    printf("get_seg_tile\n");
  }

  void get_seg_bbox(bbox& _return, const int32_t segId) {
    // Your implementation goes here
    printf("get_seg_bbox\n");
  }

  int32_t get_seg_id(const vector3d& point) {
    // Your implementation goes here
    printf("get_seg_id\n");
  }

  void get_seg_ids(std::vector<int32_t> & _return, const vector3d& point, const double radius) {
    // Your implementation goes here
    printf("get_seg_ids\n");
  }

  double compare_results(const std::vector<result> & old_results, const result& new_result) {
    // Your implementation goes here
    printf("compare_results\n");
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

