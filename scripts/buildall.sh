#!/bin/bash

cd omni.common
./bootstrap.pl 3
qmake
make && make install

cd ../omni.server
./bootstrap.pl 1
qmake
make

cd lock_server
./make.sh
