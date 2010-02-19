#!/bin/csh
#
# Install in /sbin/ssync.files
#
# This script automatically synchronizes configuration files on the
# compute nodes in a Beowulf cluster.
# 
# The host list of machines to keep in sync is /etc/ssync/hosts.
#
# The file list to push to the compute nodes is /etc/ssync/files
# 

foreach node (`cat /home/purcaro/omni.staging/scripts/cluster/hosts | grep -v ^\#`)
	ssh ${node} killall -9 omni
end

# vim:ts=4:sw=4
