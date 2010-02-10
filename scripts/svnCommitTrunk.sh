#/usr/bin/bash

cd ~/omni.trunk
svn up
svn commit
cd ~/omni.Staging
svnmerge.py merge
svn commit
cd ~/omni.trunk