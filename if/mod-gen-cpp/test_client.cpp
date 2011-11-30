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

    return 0;
}
