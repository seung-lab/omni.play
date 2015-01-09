#!/bin/bash

mkdir -p ../bin/release

g++ \
-g \
src/im_server.cpp ../thrift/src/rtm*.cpp ../thrift/src/RealTimeMesher.cpp \
-DHAVE_CONFIG_H \
-I../external/libs/boost/include \
-I../thrift/src \
-I../zi_lib \
-I. \
-I./src \
-I../external/libs/thrift/include/thrift \
-I../external/libs/thrift/include \
-L../external/libs/thrift/lib \
../external/libs/thrift/lib/libthrift.a \
../external/libs/thrift/lib/libthriftnb.a \
-lrt -lpthread -lz -levent -DNDEBUG \
-std=c++11 \
-O2 \
-o ../bin/release/rtmxz
