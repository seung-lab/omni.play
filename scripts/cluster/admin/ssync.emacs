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

foreach node (`cat /root/scripts/hosts | grep -v ^\#`)
	ssh ${node} /bin/true >& /dev/null
	setenv RETVAL $?
	if ( ${RETVAL} != 0 ) continue
	echo "Syncing ${node}"
	ssh ${node}  perl /home/purcaro/updates/emacs/updateEmacs.pl  >& /dev/null
end

# vim:ts=4:sw=4
