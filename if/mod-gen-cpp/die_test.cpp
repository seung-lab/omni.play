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
ZiARG_bool(read , 1 , "Whether we are reading or writing values to the servers");

void generate_random(char* out, int size){
        
    int i;
    for(i=0;i<size;i++){
	out[i] = rand() % 256;
	
    }

}

typedef std::pair< std::size_t, char*> pair_type;

/*
This test is meant to be run in separate parts. First it should be run with read=0 so that it stores data. Then it should be run with read=1 so that it reads
 and verifies the data. All other arguments should be the same for corresponding runs. This test is useful for testing server failures. We may store data,
 kill some servers, bring them back, and attemp to read the data.
 */

int main(int argc, char **argv){

    zi::parse_arguments(argc,argv,true);
    int size = ZiARG_size;

    boost::hash<std::string> hasher_;

    //seed random, so that we may re-use it
    srand(1);

    //get server from manager
    boost::shared_ptr<bint::storage_managerClient> manager =
	ThriftFactory<bint::storage_managerClient>::getClient(ZiARG_manager, ZiARG_m_port);

    if(!ZiARG_read)
    {

	//generate & store data
	
	std::cout << "generating and storing data" << std::endl;

	int i;
	for(i=0;i<ZiARG_num;i++){
	
	    char* data = new char[ZiARG_size]; 
	    generate_random(data,ZiARG_size);
	
	    std::size_t hash = hasher_(std::string(data,ZiARG_size));
	    

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
	    delete data;
	}

    }
    else{
	std::cout << "retrieving and verifying data" << std::endl;

	int i;
	for(i=0;i<ZiARG_num;i++){
	
	    char* data = new char[ZiARG_size]; 
	    generate_random(data,ZiARG_size);
	
	    std::size_t hash = hasher_(std::string(data,ZiARG_size));
	    

	    std::stringstream key;
	    key << hash;


	    //get data from storage server
	    bint::server_id serv_id;
	    manager->get_server(serv_id,key.str());	

	    //use the server ID gotten from the manager to open a server client
	    boost::shared_ptr<bint::storage_serverClient> s_client =
		ThriftFactory<bint::storage_serverClient>::getClient(serv_id.address, serv_id.port);

	
	    std::string ret;
	    s_client->get(ret,key.str());

	    //check that the returned value is the same
	    std::string val(data,ZiARG_size);

	    if(std::strcmp(val.c_str(),ret.c_str()) != 0){
		std::cout << "failed to get right data" << std::endl;
		std::cout << val.size() << " <-size-> " << ret.size() << std::endl;
		std::cout << "should have been: "<< val << std::endl;
		std::cout << "got: " << ret << std::endl;
		exit(1);

	    }

	    delete data;

	}
    }
	
    
	
    return 0;
}
