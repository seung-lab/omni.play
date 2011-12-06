#!/bin/bash

THRIFT=../external/libs/thrift

$THRIFT/bin/thrift -v -I . --gen cpp -o ./bin fb303/if/fb303.thrift
$THRIFT/bin/thrift -v -I . --gen cpp -o ./bin if/lock_server.thrift

GXX=g++

if [[ $(g++ --version | grep "4.6") ]]
then
    GXX=g++-4.5
fi

$GXX bin/gen-cpp/fb303_constants.cpp bin/gen-cpp/fb303_types.cpp bin/gen-cpp/FacebookService.cpp bin/gen-cpp/lock_server_constants.cpp bin/gen-cpp/lock_server_types.cpp bin/gen-cpp/lockServer.cpp fb303/FacebookBase.cpp src/main.cpp  -I./bin -I../../omni.common/src -I../external/zi_lib -I$THRIFT/include/thrift /usr/local/lib/libevent.so -pthread -lrt $THRIFT/lib/libthriftnb.a -levent_pthreads -levent_core -levent_extra -I./bin/gen-cpp -I./fb303 $THRIFT/lib/libthrift.a -o ./bin/omni.lock_server

$GXX bin/gen-cpp/fb303_constants.cpp bin/gen-cpp/fb303_types.cpp bin/gen-cpp/FacebookService.cpp bin/gen-cpp/lock_server_constants.cpp bin/gen-cpp/lock_server_types.cpp bin/gen-cpp/lockServer.cpp fb303/FacebookBase.cpp src/client.cpp  -I./bin -I../../omni.common/src -I../external/zi_lib -I$THRIFT/include/thrift /usr/local/lib/libevent.so -pthread -lrt $THRIFT/lib/libthriftnb.a -levent_pthreads -levent_core -levent_extra -I./bin/gen-cpp -I./fb303 $THRIFT/lib/libthrift.a -o ./bin/omni.lock_server_test