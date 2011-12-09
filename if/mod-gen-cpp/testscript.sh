#! /bin/bash

#script to run load_test
echo "Starting manager"
#set up manager
./manager&
sleep 1
echo "Starting server(s)"
#set up server
ID=1
PORT=9091
for i in {0..5..1}
do
    ./server --id=$ID --port=$PORT
    let "ID++"
    let "PORT++"
done
sleep 1
echo "running load_test"
#run load_test with small number of data (my disk space is small and server defaults to 1gb)
./load_test --num=100

echo "removing filemaps"
#remove filemaps
rm *filemap
echo "killing processes"
#kill processes
killall server manager