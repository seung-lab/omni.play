INCFLAGS	=	-I/usr/include/thrift -I/Users/zlateski/zi_libx/external/include -I/Users/zlateski/zi_libx
LINKFLAGS	=	-lpthread -lthrift

all:
	g++ storage_manager_server.skeleton.cpp storage_manager.cpp filesystem_constants.cpp filesystem_types.cpp $(INCFLAGS) $(LINKFLAGS) -o manager
	g++ storage_server.skeleton.cpp storage_server.cpp filesystem_constants.cpp filesystem_types.cpp $(INCFLAGS) $(LINKFLAGS) -o server

clean:
	rm -rf server manager