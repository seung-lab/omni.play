# LIBRARY

#specify sources
SET(OM_UTILITY_LIB_SRCS
	omHdf5.cpp
	omHdf5Dataset.cpp
	omHdf5Helpers.cpp
	omHdf5LowLevel.cpp
	omHdf5LowLevelWrappersAutoOpenClose.cpp
	omHdf5LowLevelWrappersManualOpenClose.cpp
	omImageDataIo.cpp
	omSystemInformation.cpp
	localPrefFiles.cpp
)

#create static library
ADD_LIBRARY(omUtility
	${OM_UTILITY_LIB_SRCS}
)

TARGET_LINK_LIBRARIES( omUtility
	omCommon
	omLibrary
	${OM_VTK_LIBS}
	${OM_HDF5_LIBS}
	${OM_BOOST_LIBS}
)

# EXECUTABLE

#specify executable
#ADD_EXECUTABLE( omUtilityApp
#	main.cpp
#)

#link binaries targets against libraries
#TARGET_LINK_LIBRARIES( omUtilityApp 
#	omUtility
#)
