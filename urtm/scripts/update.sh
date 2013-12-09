#!/bin/bash

for f in `cat volumes.txt`; do
    uploadConsensusChild.sh $f
done