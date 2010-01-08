
# LIBRARY

#specify sources
SET(OM_UTILITY_LIB_SRCS
	omHdf5.cpp
	omImageDataIo.cpp
)


#create static library
ADD_LIBRARY(omUtility 
	${OM_UTILITY_LIB_SRCS}
)



TARGET_LINK_LIBRARIES( omUtility
	omCommon
	omSystem
	omLibrary
	omView2d
	${HDF5_LIBRARY}
)






# EXECUTABLE

#specify executable
ADD_EXECUTABLE( omUtilityApp
	main.cpp
)

#link binaries targets against libraries
TARGET_LINK_LIBRARIES( omUtilityApp 
	omUtility
)
