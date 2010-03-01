#!/bin/csh

foreach node (`cat /home/purcaro/omni.staging/scripts/cluster/hosts | grep -v ^\#`)
	echo "Removing file from ${node}"
	foreach file ("/tmp/meshinator.mesh")
		if ( -e ${file} ) then
			echo "    Unlinking ${file}" ;
			rm ${file} ;
		endif
	end
end

# vim:ts=4:sw=4
