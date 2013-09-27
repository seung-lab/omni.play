#include <zi/arguments.hpp>

#include "serverHandler.hpp"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TThreadPoolServer.h>
#include <server/TNonblockingServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <concurrency/ThreadManager.h>
#include <concurrency/PosixThreadFactory.h>
#include <boost/shared_ptr.hpp>

#include "zi/system/daemon.hpp"

ZiARG_int32(port, 9090, "Server's port");
ZiARG_bool(daemonize, true, "Run as daemon");
ZiARG_string(mesher, "localhost", "Mesher's address");
ZiARG_int32(mport, 9090, "Mesher's port");

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using namespace ::om::server;

int main(int argc, char *argv[]) {
  zi::parse_arguments(argc, argv, true);
  if (ZiARG_daemonize) {
    if (!::zi::system::daemonize(true, true)) {
      log_errors(unknown) << "Error trying to daemonize.";
      return -1;
    }
  }

  int port = ZiARG_port;
  boost::shared_ptr<serverHandler> handler(
      new serverHandler(ZiARG_mesher, ZiARG_mport));
  boost::shared_ptr<TProcessor> processor(new serverProcessor(handler));
  boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  boost::shared_ptr<TTransportFactory> transportFactory(
      new TBufferedTransportFactory());
  boost::shared_ptr<TProtocolFactory> protocolFactory(
      new TBinaryProtocolFactory());

  boost::shared_ptr<ThreadManager> threadManager =
      ThreadManager::newSimpleThreadManager(32);
  auto threadFactory = boost::make_shared<PosixThreadFactory>();

  threadManager->threadFactory(threadFactory);
  threadManager->start();

  auto server = boost::make_shared<TNonblockingServer>(
      processor, transportFactory, transportFactory, protocolFactory,
      protocolFactory, port, threadManager);

  handler->setServer(server);                // For Service Shutdown
  handler->setThreadManager(threadManager);  // For thread busyness checking
  server->serve();
  return 0;
}
