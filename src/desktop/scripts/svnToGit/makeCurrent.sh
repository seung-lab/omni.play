#!/bin/bash

# automake import of current Omni src into github
# purcaro@gmail.com  Feb 2012

DIR=/tmp/current

rm -rf $DIR
mkdir $DIR
cd $DIR

perl -c /home/mjp/omni/rewritePaths.pl
perl -c /home/mjp/omni/walkSVN.pl

SVNB=/home/mjp/bin/subversion-1.7.3/bin

if [ 0 == 1 ]; then 
    # use cloned repos....
    echo "copying repos..."
    cp -a /home/mjp/omni/netgpu ./

    echo "fix authorship, and add revision number to every commit..."
    /home/mjp/omni/walkSVN.pl $DIR

    # dump repos
    echo "dumping full repos..."
    $SVNB/svnadmin dump netgpu > /home/mjp/omni/netgpu.script.dump
else
    echo "symlink  fixed dump file..."
    ln -s /home/mjp/omni/netgpu.script.dump
fi 

# make new svn dump file, including only omni project folder
echo "extracting omni part of repos..."
$SVNB/svndumpfilter include omni --drop-empty-revs --renumber-revs < netgpu.script.dump > omni.orig.dump

echo "removing external binary files, zi_libs, etc."

if [ 0 == 1 ]; then 
    cat omni.orig.dump | $SVNB/svndumpfilter exclude --pattern '*/external/tarballs/*' --quiet  > omni.noExtTarBalls.dump
    cat omni.noExtTarBalls.dump | $SVNB/svndumpfilter exclude --pattern '*/zi_lib/external/*' --quiet  > omni.noZiExt.dump
    cat omni.noZiExt.dump | $SVNB/svndumpfilter exclude --pattern '*/external/GoogleProfiler/*' --quiet  > omni.noGoogleProf.dump
    cat omni.noGoogleProf.dump | $SVNB/svndumpfilter exclude --pattern '*/external/headers/*' --quiet  > omni.noHeaders.dump
    cat omni.noHeaders.dump | $SVNB/svndumpfilter exclude --pattern '*/scripts/cluster/updates/*'  --quiet  > omni.noCluster.dump
    cat omni.noCluster.dump | $SVNB/svndumpfilter exclude --pattern '*/omniNetbeans/*' --quiet  > omni.noNetbeans.dump
    cat omni.noNetbeans.dump | $SVNB/svndumpfilter exclude --pattern '*/omni/include/zi_lib/*'  --quiet  > omni.noOldZi.dump
    cat omni.noOldZi.dump | $SVNB/svndumpfilter exclude --pattern '*/external/zi_lib/*' --quiet  > omni.noZiLibAtAll.dump
    cat omni.noZiLibAtAll.dump | $SVNB/svndumpfilter exclude --pattern '*/omni/include/zi_lib_v2/*'  --quiet  > omni.noOldZiV2.dump
elif [ 0 == 1 ]
BASE=$SVNB/svndumpfilter --quiet exclude --pattern
  cat omni.orig.dump | $BASE '*/external/tarballs/*' | $BASE '*/zi_lib/external/*' | $BASE '*/external/GoogleProfiler/*' | $BASE '*/external/headers/*' | $BASE '*/scripts/cluster/updates/*' | $BASE '*/omniNetbeans/*' | $BASE '*/omni/include/zi_lib/*' | $BASE '*/external/zi_lib/*' | $BASE '*/omni/include/zi_lib_v2/*'  > omni.noOldZiV2.dump

fi

ln -s omni.noOldZiV2.dump omni.clean.badStructure.dump

echo "fixing svn folder structure..."
/home/mjp/omni/rewritePaths.pl $DIR

echo "importing into fresh svn repos..."
$SVNB/svnadmin create omni
$SVNB/svnadmin load omni < omni.clean.dump

echo "importing into git..."
git svn clone file://$DIR/omni --no-metadata -s git

cd git/

# refs/remotes => refs/heads
# from http://stackoverflow.com/questions/1584675/converting-svn-to-git-how-to-get-the-branches-not-to-be-just-remote-in-the-svn

`perl -i -pe 's/refs\/remotes/refs\/heads/g' .git/packed-refs`

#git remote add origin git@github.com:purcaro/test.git
#git push origin --all
