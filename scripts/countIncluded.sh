#!/bin/bash

for header in `find desktop/src -regex '.*\.h\(pp\)?'`
do
    echo $header | sed 's/.*\///' | xargs -I {} grep -R {} desktop/src | wc -l | xargs echo -n
    echo -e "\t" $header
done | sort -n
