

## LIBRARY ##

#specify sources
SET(OM_VOLUME_LIB_SRCS
	omChannel.cpp
	omDataVolume.cpp
	omMipChunk.cpp
	omMipChunkCoord.cpp
	omVolumeCuller.cpp
	omMipVolume.cpp
	omSegmentation.cpp
	omVolume.cpp
	../utility/dataWrappers.cpp
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
)






## EXECUTABLE ##

#specify executable
#ADD_EXECUTABLE( omVolumeApp
#	main.cpp
#)


#TARGET_LINK_LIBRARIES( omVolumeApp
#	omVolume
#)
