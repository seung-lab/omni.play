#include "serverHandler.hpp"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TThreadPoolServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <concurrency/ThreadManager.h>
#include <concurrency/PosixThreadFactory.h>
#include <boost/shared_ptr.hpp>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

using namespace ::om::server;

int main(int argc, char *argv[])
{
    int port = 9090;
    boost::shared_ptr<serverHandler> handler(new serverHandler());
    boost::shared_ptr<TProcessor> processor(new serverProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    boost::shared_ptr<ThreadManager> threadManager(ThreadManager::newSimpleThreadManager(32));
    boost::shared_ptr<PosixThreadFactory> threadFactory(new PosixThreadFactory());

    threadManager->threadFactory(threadFactory);
    threadManager->start();

    TThreadPoolServer server(processor, serverTransport, transportFactory,
                             protocolFactory, threadManager);
    server.serve();
    return 0;
}
