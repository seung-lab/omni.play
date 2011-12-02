#include "simple_client.hpp"
#include "storage_manager.h"
#include "storage_server.h"
#include <iostream>

int main(int argc, char **argv){

    std::string key = "key1";
    std::string val = "hello world";

    //get server from manager
    boost::shared_ptr<bint::storage_managerClient> m_client = 
	ThriftFactory<bint::storage_managerClient>::getClient("localhost", 9090);

    bint::server_id serv_id;
    m_client->get_server(serv_id,key);

    std::cout << "server for key is in: "<< serv_id.address << ":" << serv_id.port << std::endl;

    //use the server ID gotten from the manager to open a server client
    boost::shared_ptr<bint::storage_serverClient> s_client = 
	ThriftFactory<bint::storage_serverClient>::getClient(serv_id.address, serv_id.port);

    std::cout << "attempting to find key in server\n";
    std::string ret1;
    s_client->get(ret1,key);
    std::cout << "found: "<<ret1<<std::endl;

    s_client->put(key,val);
    std::cout << "put value in storage with key: "<<key<<std::endl;
    std::string ret2;
    s_client->get(ret2,key);
    std::cout << "got back value: "<<ret2<<std::endl;

    return 0;
}
