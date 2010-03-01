#!/bin/csh

foreach node (`cat /home/purcaro/omni.staging/scripts/cluster/hosts | grep -v ^\#`)
	echo "Syncing ${node}"
	foreach file (`cat /home/purcaro/omni.staging/scripts/cluster/files | grep -v ^\#`)
		if ( -e ${file} ) then
			echo "    Copying ${file}" ;
			rsync -a ${file} ${node}:${file} ;
		endif
	end
end

# vim:ts=4:sw=4
