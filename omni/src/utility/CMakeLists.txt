# LIBRARY

#specify sources
SET(OM_UTILITY_LIB_SRCS
	fileHelpers.cpp
	omHdf5.cpp
	omDataLayer.cpp
	omHdf5Reader.cpp
	omHdf5Writer.cpp
	omDummyWriter.cpp
	omHdf5Manager.cpp
	omHdf5Dataset.cpp
	omHdf5Path.cpp
	omHdf5Helpers.cpp
	omHdf5LowLevel.cpp
	omHdf5LowLevelWrappersAutoOpenClose.cpp
	omHdf5LowLevelWrappersManualOpenClose.cpp
	omImageDataIo.cpp
	omSystemInformation.cpp
	localPrefFiles.cpp
	sortHelpers.cpp
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
ADD_EXECUTABLE( hdf5_merge
        hdf5_merge.cpp
)
TARGET_LINK_LIBRARIES( hdf5_merge 
	${OM_HDF5_LIBS}
	-lpthread
)

#specify executable
ADD_EXECUTABLE( h5get
        h5get.cpp
)
TARGET_LINK_LIBRARIES( h5get 
	${OM_HDF5_LIBS}
	-lpthread
)

#specify executable
ADD_EXECUTABLE( h5meshget
        h5meshget.cpp
)
TARGET_LINK_LIBRARIES( h5meshget 
	${OM_HDF5_LIBS}
	-lpthread
)


#specify executable
#ADD_EXECUTABLE( omUtilityApp
#	main.cpp
#)

#link binaries targets against libraries
#TARGET_LINK_LIBRARIES( omUtilityApp 
#	omUtility
#)
