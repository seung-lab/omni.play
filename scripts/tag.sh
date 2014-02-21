#!/bin/bash

project="$1"
if [[ ! -d $project || ! -e $project/version/major || ! -e $project/version/minor ]]; then
  echo "Invalid project"
  exit 1;
fi

version_major=`cat $project/version/major`
version_minor=`cat $project/version/minor`
version=$version_major"."$version_minor
tag=$project"-"$version

git tag -a $tag -m ''

echo $((version_minor+1)) > $project/version/minor
git add $project/version/minor
git commit -m "Update minor version."
git push origin --tags
