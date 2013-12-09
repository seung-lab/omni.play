mkdir -p ../bin/release

g++ \
-g \
src/im_server.cpp ../thrift/src/*.cpp \
-DHAVE_CONFIG_H \
-I../external/libs/boost/include \
-I../thrift/src \
-I. \
-I./src \
-I../external/libs/thrift/include/thrift \
-I../external/libs/thrift/include \
-L../external/libs/thrift/lib \
-lrt -lpthread -lthrift -lz -lthriftnb -levent -DNDEBUG \
-O2 \
-o ../bin/release/rtmxz
