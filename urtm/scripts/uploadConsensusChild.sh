#!/bin/bash

MESHER="18.4.45.150"
MPORT="9999"
E2198_ROOT="/omniData/e2198"

CELL=`cd $(dirname $1) && basename $(pwd) | grep -Po '\d+'`
VOLUME_TAG=`basename $1 | grep -Po '^[^_]+'`
DIR=$(ls -d ${E2198_ROOT}/*_${VOLUME_TAG}_*.omni.files)
SEGS=`cat $1 | cut -d' ' -f1 | xargs | sed 's/ /,/g'`

omni.urtm --mesher=$MESHER --mport=$MPORT --cell=$CELL --path=$DIR --segs=$SEGS