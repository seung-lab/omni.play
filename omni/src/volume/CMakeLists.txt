## LIBRARY ##

#specify sources
SET(OM_VOLUME_LIB_SRCS
	omChannel.cpp
	omDataVolume.cpp
	omFilter2d.cpp
	omFilter2dManager.cpp
	omMipChunk.cpp
	omMipChunkCoord.cpp
	omSegmentationChunkCoord.cpp
	omVolumeCuller.cpp
	omMipVolume.cpp
	omSegmentation.cpp
	omVolume.cpp
	../utility/dataWrappers.cpp
	../mesh/meshingManager.cpp
	../mesh/meshingChunkThreadManager.cpp
	../mesh/meshingChunkThread.cpp
)


#create static library
ADD_LIBRARY(omVolume 
	${OM_VOLUME_LIB_SRCS}
)


TARGET_LINK_LIBRARIES( omVolume

	omCommon
	omSystem
	omSegment
	omMesh
	omVoxel
	omView2d
)


## EXECUTABLE ##

#specify executable
#ADD_EXECUTABLE( omVolumeApp
#	main.cpp
#)


#TARGET_LINK_LIBRARIES( omVolumeApp
#	omVolume
#)
