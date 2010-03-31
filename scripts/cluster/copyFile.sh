#!/bin/csh

foreach node (`cat /home/purcaro/omni.staging/scripts/cluster/hosts | grep -v ^\#`)
	echo "Syncing ${node}"
	rsync --partial --progress /scratch/purcaro/Omni-data/E1088--justSegmentationNoMesh.omni ${node}:/scratch/purcaro/Omni-data/E1088--justSegmentationNoMesh.omni
end

# vim:ts=4:sw=4
