INCFLAGS	=	-I/usr/include/thrift -I/home/zlateski/code/zi_lib/external/include -I/home/zlateski/code/zi_lib
#INCFLAGS	=	-I/usr/local/include/thrift -I/usr/local/zi_lib/external/include -I/usr/local/zi_lib
LINKFLAGS	=	-lpthread -lthrift -ggdb -lrt -Wextra
.PHONY:
all:
	g++-4.5 storage_manager_server.skeleton.cpp storage_manager.cpp filesystem_constants.cpp filesystem_types.cpp $(INCFLAGS) $(LINKFLAGS) -o manager
	g++-4.5 storage_server.skeleton.cpp storage_server.cpp filesystem_constants.cpp filesystem_types.cpp storage_manager.cpp $(INCFLAGS) $(LINKFLAGS) -o server
	g++-4.5 test_client.cpp storage_manager.cpp filesystem_constants.cpp filesystem_types.cpp storage_server.cpp $(INCFLAGS) $(LINKFLAGS) -o test_client
.PHONY:
clean:
	rm -rf server manager test_client *.filemap

.PHONY:
remake:	clean all