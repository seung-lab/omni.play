#include "simple_client.hpp"
#include "storage_manager.h"
#include "storage_server.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <zi/arguments.hpp>
#include <zi/utility/for_each.hpp>
#include <boost/functional/hash.hpp>
#include <stdlib.h>
#include <time.h>
#include <sstream>

ZiARG_int32(size, 128*128*128 , "The size of the generated data");
ZiARG_int32(num, 10000 , "The number of data entries to generate");
ZiARG_string(manager, "localhost", "Manager's IP");
ZiARG_int32(m_port, 9090, "Manager's port");

void generate_random(char* out, int size){
        
    int i;
    for(i=0;i<size;i++){
	out[i] = rand() % 256;
    }

}

typedef std::pair< std::size_t, char*> pair_type;

int main(int argc, char **argv){

    zi::parse_arguments(argc,argv,true);
    int size = ZiARG_size;

    boost::hash<std::string> hasher_;

    //seed random
    srand(time(NULL));

    //get server from manager
    boost::shared_ptr<bint::storage_managerClient> manager =
	ThriftFactory<bint::storage_managerClient>::getClient(ZiARG_manager, ZiARG_m_port);

    //generate & store data
    std::vector< pair_type > list;
	
    int i;
    for(i=0;i<ZiARG_num;i++){
	
	char data[ZiARG_size]; 
	generate_random(data,ZiARG_size);
	
	std::size_t hash = hasher_(std::string(data,ZiARG_size));
	list.push_back(std::pair<std::size_t,char*> (hash,data));

	std::stringstream key;
	key << hash;
	std::string val(data,ZiARG_size);


	bint::server_id serv_id;
	manager->get_server(serv_id,key.str());	

	//use the server ID gotten from the manager to open a server client
	boost::shared_ptr<bint::storage_serverClient> s_client =
	    ThriftFactory<bint::storage_serverClient>::getClient(serv_id.address, serv_id.port);

	//put hash-data pair in storage server
	s_client->put(key.str(),val);

    }


    
	
    
	
    return 0;
}
