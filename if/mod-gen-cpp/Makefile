all:
	g++ storage_server.skeleton.cpp storage_server.cpp filesystem_constants.cpp filesystem_types.cpp -lpthread -lthrift -I/usr/local/include/thrift -I/usr/local/lib -o server

	g++ storage_manager_server.skeleton.cpp storage_manager.cpp filesystem_constants.cpp filesystem_types.cpp -lpthread -lthrift -I/usr/local/include/thrift -I/usr/local/lib -o manager
