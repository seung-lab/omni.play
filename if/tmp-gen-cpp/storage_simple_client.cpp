#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

#include <boost/shared_ptr.hpp>
#include "storage_server.h"

#include <string>
#include <iostream>

template <class _CLI>
struct ThriftFactory {
    static boost::shared_ptr<_CLI>
    getClient(std::string host,
              int32_t     port,
              int32_t     connTmo = 6000000,
              int32_t     sendTmo = 6000000,
              int32_t     recvTmo = 6000000);
};

template<class _CLI>
boost::shared_ptr<_CLI>
ThriftFactory<_CLI>::getClient(std::string host,
                               int32_t     port,
                               int32_t     connTmo,
                               int32_t     sendTmo,
                               int32_t     recvTmo)
{
    boost::shared_ptr<apache::thrift::transport::TSocket> socket =
        boost::shared_ptr<apache::thrift::transport::TSocket>
        (new apache::thrift::transport::TSocket(host, port));

    socket->setConnTimeout(connTmo);
    socket->setSendTimeout(sendTmo);
    socket->setRecvTimeout(recvTmo);

    boost::shared_ptr<apache::thrift::transport::TTransport> transport =
        boost::shared_ptr<apache::thrift::transport::TTransport>
        (new apache::thrift::transport::TBufferedTransport(socket));

    boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol  =
        boost::shared_ptr<apache::thrift::protocol::TProtocol>
        (new apache::thrift::protocol::TBinaryProtocol(transport));

    boost::shared_ptr<_CLI> client(new _CLI(protocol));

    try {
        transport->open();
    } catch (apache::thrift::TException &tx) {
        //throw(tx);MEXPRINTF("%s", tx.what());
        return client;
    }
    return client;
}


int main()
{
    boost::shared_ptr<bint::storage_serverClient> client
        = ThriftFactory<bint::storage_serverClient>::getClient("localhost", 9090);

    client->put("aleks", "1");


    std::string r;

    client->get(r, "aleks");
    std::cout << r << "\n";
}
