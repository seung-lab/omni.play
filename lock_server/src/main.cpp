#include <zi/rwlock_server.hpp>

#include "gen-cpp/lockServer.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TNonblockingServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <concurrency/ThreadManager.h>
#include <concurrency/PosixThreadFactory.h>
#include "FacebookBase.h"

#include <boost/shared_ptr.hpp>

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
    zi::rwlock_server<std::string,std::string,97> locks_;

public:
    lockServerHandler()
        : facebook::fb303::FacebookBase("Lock Server")
        , omni::server::lockServerIf()
        , locks_()
    { }

    void acquire(const std::string& client_id, const std::string& lock_id)
    {
        printf("acquire\n");
        locks_.acquire_write(client_id,lock_id);
    }

    void acquireRead(const std::string& client_id, const std::string& lock_id)
    {
        printf("acquireRead\n");
        locks_.acquire_read(client_id,lock_id);
    }

    void acquireWrite(const std::string& client_id, const std::string& lock_id)
    {
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


int main()
{
    int port = 9090;
    boost::shared_ptr<TProtocolFactory>   protocolFactory(new TBinaryProtocolFactory());
    boost::shared_ptr<lockServerHandler>  handler(new lockServerHandler());
    boost::shared_ptr<TProcessor>         processor(new omni::server::lockServerProcessor(handler));
    boost::shared_ptr<ThreadManager>      threadManager(ThreadManager::newSimpleThreadManager(100));
    boost::shared_ptr<PosixThreadFactory> threadFactory(new PosixThreadFactory());

    threadManager->threadFactory(threadFactory);
    threadManager->start();

    boost::shared_ptr<TNonblockingServer> server
        ( new TNonblockingServer(processor, protocolFactory, port, threadManager));

    handler->setServer(server);

    server->serve();
}
