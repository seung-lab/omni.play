#!/bin/bash
thrift -v -I . --gen cpp -o ./bin fb303/if/fb303.thrift
thrift -v -I . --gen cpp -o ./bin if/lock_server.thrift
g++-4.5 bin/gen-cpp/fb303_constants.cpp bin/gen-cpp/fb303_types.cpp bin/gen-cpp/FacebookService.cpp bin/gen-cpp/lock_server_constants.cpp bin/gen-cpp/lock_server_types.cpp bin/gen-cpp/lockServer.cpp fb303/FacebookBase.cpp src/main.cpp  -I./bin -I../../omni.common/src -I../../omni.common/external/zi_lib -I/usr/include/thrift -levent -pthread -lrt -lthriftnb -levent_pthreads -levent_core -levent_extra -I./bin/gen-cpp -I./fb303 -lthrift -o ./bin/lock_server
g++-4.5 bin/gen-cpp/fb303_constants.cpp bin/gen-cpp/fb303_types.cpp bin/gen-cpp/FacebookService.cpp bin/gen-cpp/lock_server_constants.cpp bin/gen-cpp/lock_server_types.cpp bin/gen-cpp/lockServer.cpp fb303/FacebookBase.cpp src/client.cpp  -I./bin -I../../omni.common/src -I../../omni.common/external/zi_lib -I/usr/include/thrift -levent -pthread -lrt -lthriftnb -levent_pthreads -levent_core -levent_extra -I./bin/gen-cpp -I./fb303 -lthrift -o ./bin/lock_client