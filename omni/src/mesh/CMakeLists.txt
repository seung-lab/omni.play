
# LIBRARY

#specify sources
SET(OM_MESH_LIB_SRCS
	
	omMipChunkMesher.cpp
	omMesher.cpp
	omMipMesh.cpp
	omMipMeshManager.cpp
	omMipMeshCoord.cpp
)


#create static library
ADD_LIBRARY(omMesh 
	${OM_MESH_LIB_SRCS}
)



TARGET_LINK_LIBRARIES( omMesh
	omCommon
	omSegment
	omVolume
	omSystem
)






# EXECUTABLE

#specify executable
#ADD_EXECUTABLE(omMeshApp
#	main.cpp
#)

#link binaries targets against libraries
#TARGET_LINK_LIBRARIES( omMeshApp 
#	omMesh
#)
