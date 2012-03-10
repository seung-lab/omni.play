#! /bin/bash


# #script to run load_test
# echo "Running simple load test"

# echo "Starting manager"
# #set up manager
# ./manager&
# sleep 1
# echo "Starting server(s)"
# #set up server
# ID=1
# PORT=9091
# for i in {0..5..1}
# do
#     ./server --id=$ID --port=$PORT
#     let "ID++"
#     let "PORT++"
# done
# sleep 1
# echo "running load_test"
# #run load_test with small number of data (my disk space is small and server defaults to 1gb)
# ./load_test --num=100

# echo "removing filemaps"
# #remove filemaps
# rm *filemap
# echo "killing processes"
# #kill processes
# killall server manager

#script to run server fault tolerance test
echo "Running server fault tolerance test"

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

echo "running die_test for storage"
./die_test --num=100 --read=0

echo "Killing servers"
killall server

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

echo "running die_test for retrieval"
./die_test --num=100

echo "removing filemaps"
#remove filemaps
rm *filemap
echo "killing processes"
#kill processes
killall server manager