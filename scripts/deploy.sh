#!/bin/bash

usage()
{
cat << EOF
usage: $0 -s [ip address] options

Deploys Omni on a given machine. Must include one of the deployment options.

OPTIONS:
   -h      Show this message
   -q      Deploy MySQL
   -l      Deploy Locking Server
   -o      Deploy Omni.server
EOF
}

SERVER=
MYSQL=
LOCKING=
OMNI=
while getopts “hs:qlo” OPTION
do
     case $OPTION in
         h)
             usage
             exit 1
             ;;
         s)
             SERVER=$OPTARG
             ;;
         q)
             MYSQL=1
             ;;
         l)
             LOCKING=1
             ;;
         o)
             OMNI=1
             ;;
         ?)
             usage
             exit
             ;;
     esac
done

if [[ -z $MYSQL ]] && [[ -z $LOCKING ]] && [[ -z $OMNI ]]
then
     usage
     exit 1
fi

if [[ -z $SERVER ]]
then
    usage
    exit 1
fi

if [[ $MYSQL ]]
then

fi

if [[ $LOCKING ]]
then

fi

if [[ $OMNI ]]
then
    scp
fi