#INCFLAGS	=	-I/usr/include/thrift -I/Users/zlateski/zi_libx/external/include -I/Users/zlateski/zi_libx
INCFLAGS	=	-I/usr/local/include/thrift -I/usr/local/zi_lib/external/include -I/usr/local/zi_lib
LINKFLAGS	=	-lpthread -lthrift
.PHONY:
all:
	g++ storage_manager_server.skeleton.cpp storage_manager.cpp filesystem_constants.cpp filesystem_types.cpp $(INCFLAGS) $(LINKFLAGS) -o manager
	g++ storage_server.skeleton.cpp storage_server.cpp filesystem_constants.cpp filesystem_types.cpp storage_manager.cpp $(INCFLAGS) $(LINKFLAGS) -o server
	g++ test_client.cpp storage_manager.cpp filesystem_constants.cpp filesystem_types.cpp storage_server.cpp $(INCFLAGS) $(LINKFLAGS) -o test_client
.PHONY:
clean:
	rm -rf server manager test_client