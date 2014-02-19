#!/bin/bash

project="$1"
version_major=`cat $project/version/major`
version_minor=`cat $project/version/minor`
version=$version_major"."$version_minor
tag=$project"-"$version

git tag -a $tag -m ''
git push origin --tags

echo $version_minor+1 > $project/version/minor