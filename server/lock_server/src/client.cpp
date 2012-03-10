#include "gen-cpp/lockServer.h"
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <map>
#include <string>
#include <zi/utility/for_each.hpp>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using boost::shared_ptr;

int main()
{

    shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    shared_ptr<TProtocol>  protocol(new TBinaryProtocol(transport));
    omni::server::lockServerClient client(protocol);

    transport->open();

    client.acquireWrite("a", "b");
    //std::cout << client.tryAcquireWrite("a","b") << std::endl;
    //std::cout << client.tryAcquireRead("a","b") << std::endl;

    //sleep(1);

    client.releaseWrite("a", "b");

    //if ( client.tryAcquireWrite("a","b") )
    // {
    //     client.acquireWrite("a", "b");
    //     std::cout << "Got Write" << std::endl;
    //     client.releaseWrite("a", "b");
    // }
    // //else
    // {
    //     //std::cout << "Didnt Get Write" << std::endl;
    // }

    client.acquireRead("a", "b");
    client.acquireRead("a", "b");
    // //std::cout << client.tryAcquireRead("a","b") << std::endl;
    // //client.releaseRead("a", "b");
    client.releaseRead("a", "b");
    client.releaseRead("a", "b");

    client.acquireRead("a", "b");
    // //std::cout << client.tryAcquireRead("a","b") << std::endl;
    // //client.releaseRead("a", "b");
    client.releaseRead("a", "b");

    std::map<std::string, int64_t> r;
    client.getCounters(r);

    FOR_EACH(it, r)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }


}
