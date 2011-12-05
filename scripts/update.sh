#!/bin/bash

cd omni.common
git pull
qmake
make -j16 && make install
cp bin/libomni.common.a lib/bin

cd ../omni.server
git pull
qmake
make -j16