#include <zi/rwlock_server.hpp>
#include <zi/system/daemon.hpp>
#include <zi/arguments.hpp>

#include "gen-cpp/lockServer.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TNonblockingServer.h>
#include <server/TThreadPoolServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <concurrency/ThreadManager.h>
#include <concurrency/PosixThreadFactory.h>
#include "FacebookBase.h"

#include <boost/shared_ptr.hpp>

#include <iostream>

ZiARG_int32(port, 9090, "Server's port");
ZiARG_bool(daemonize, true, "Run as daemon");

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using apache::thrift::concurrency::ThreadManager;
using apache::thrift::concurrency::PosixThreadFactory;

class lockServerHandler
    : virtual public omni::server::lockServerIf
    , virtual public facebook::fb303::FacebookBase
{
private:
    typedef facebook::fb303::FacebookBase         fb303 ;
    zi::rwlock_server<std::string,std::string,97> locks_;

public:
    lockServerHandler()
        : facebook::fb303::FacebookBase("Lock Server")
        , omni::server::lockServerIf()
        , locks_()
    { }

    void acquire(const std::string& client_id, const std::string& lock_id)
    {
        fb303::incrementCounter("acquireWrite");
        printf("acquire\n");
        locks_.acquire_write(client_id,lock_id);
    }

    void acquireRead(const std::string& client_id, const std::string& lock_id)
    {
        fb303::incrementCounter("acquireRead");
        printf("acquireRead\n");
        locks_.acquire_read(client_id,lock_id);
    }

    void acquireWrite(const std::string& client_id, const std::string& lock_id)
    {
        fb303::incrementCounter("acquireWrite");
        printf("acquireWrite\n");
        locks_.acquire_write(client_id,lock_id);
    }

    void release(const std::string& client_id, const std::string& lock_id)
    {
        printf("release\n");
        locks_.release_write(client_id,lock_id);
    }

    void releaseRead(const std::string& client_id, const std::string& lock_id)
    {
        printf("releaseRead\n");
        locks_.release_read(client_id,lock_id);
    }

    void releaseWrite(const std::string& client_id, const std::string& lock_id)
    {
        printf("releaseWrite\n");
        locks_.release_write(client_id,lock_id);
    }

    bool tryAcquireRead(const std::string& client_id, const std::string& lock_id)
    {
        printf("tryAcquireRead\n");
        return locks_.try_acquire_read(client_id,lock_id);
    }

    bool tryAcquireWrite(const std::string& client_id, const std::string& lock_id)
    {
        printf("tryacquireWrite\n");
        return locks_.try_acquire_write(client_id,lock_id);
    }

    facebook::fb303::fb_status::type getStatus()
    {
        return facebook::fb303::fb_status::ALIVE;
    }

};


int main(int argc, char *argv[])
{
    zi::parse_arguments(argc, argv, true);
    if ( ZiARG_daemonize )
    {
        if(!::zi::system::daemonize(true, true)) {
            std::cerr << "Error trying to daemonize." << std::endl;
            return -1;
        }
    }

    int port = ZiARG_port;
    boost::shared_ptr<lockServerHandler>  handler(new lockServerHandler());
    boost::shared_ptr<TProcessor>         processor(new omni::server::lockServerProcessor(handler));
    boost::shared_ptr<TServerTransport>   serverTransport(new TServerSocket(ZiARG_port));
    boost::shared_ptr<TTransportFactory>  transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory>   protocolFactory(new TBinaryProtocolFactory());

    boost::shared_ptr<ThreadManager>      threadManager(ThreadManager::newSimpleThreadManager(100));
    boost::shared_ptr<PosixThreadFactory> threadFactory(new PosixThreadFactory());

    threadManager->threadFactory(threadFactory);
    threadManager->start();


    boost::shared_ptr<TServer> server(new TThreadPoolServer(processor,
                                                                      serverTransport,
                                                                      transportFactory,
                                                                      protocolFactory,
                                                                      threadManager));
    // boost::shared_ptr<TNonblockingServer> server
    //     ( new TNonblockingServer(processor, protocolFactory, port, threadManager));

    handler->setServer(server);

    server->serve();
}
