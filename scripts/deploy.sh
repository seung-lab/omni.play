#!/bin/bash

usage()
{
cat << EOF
usage: $0 head_node_hostname

EOF
}

if [[ $# -ne 1 ]]
then
    usage
    exit(-1)
fi

HEAD=$1
OMNI=/usr/local/omni

#add NFS mount as necessary
if [[ -z $(grep "$HEAD:$OMNI") ]]
then
    mkdir /usr/local/omni
    echo "$HEAD:$OMNI $OMNI nfs rw,hard,intr,auto" >> /etc/fstab
    mount -a
fi

# disable the node
rm -f /etc/apache2/sites-enabled/

# update web content
rsync $OMNI/omni-web /var/www/omni-web
cp -f $OMNI/omni-web/omniweb /etc/apache2/sites-available/

# stop and restart the server to run latest version
killall omni.server
$OMNI/omni.server/bin/omni.server

#reenable the site
ln -s /etc/apache2/sites-available/omniweb /etc/apache2/sites-enabled/
