
# LIBRARY

#specify sources
SET(OM_VOXEL_LIB_SRCS

	omMipVoxelation.cpp
	omMipVoxelationManager.cpp
)


#create static library
ADD_LIBRARY(omVoxel
	${OM_VOXEL_LIB_SRCS}
)



TARGET_LINK_LIBRARIES( omVoxel
	omCommon
	omSegment
	omVolume
	omSystem
)






# EXECUTABLE

#specify executable
#ADD_EXECUTABLE(omVoxelApp
#	main.cpp
#)

#link binaries targets against libraries
#TARGET_LINK_LIBRARIES( omVoxelApp 
#	omVoxel
#)
