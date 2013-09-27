#!/bin/bash
for header in $(find desktop/src -regex ".*\.h\(pp\)?")
do
g++ -I. -I./common/src -I./common/include\
    -I./common/include/yaml-cpp/include\
    -I./external/libs/libjpeg/include\
    -I./zi_lib -I./common/include/libb64/include\
    -I./external/libs/boost/include -I./desktop/src\
    -I./desktop/include -I./desktop/lib -I./desktop\
    -I./external/libs/qt/include/Qt\
    -I./external/libs/qt/include/QtCore\
    -I./external/libs/qt/include/QtOpenGL\
    -I./external/libs/qt/include/QtGui\
    -I./external/libs/qt/include/QtNetwork\
    -I./external/libs/qt/include\
    -I./external/libs/hdf5/include\
    -I./common/include/libb64/include\
    -I./external/srcs/google-breakpad/src\
    -M $header | sed -e 's/\\$//' -e 's/^.*://' -e 's/\s//' | grep -v "^external" | grep -v "zi_lib" | grep -v "/usr"
done | sort | uniq -c | sort -n
