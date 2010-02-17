
# LIBRARY

#specify sources
SET(OM_PROJECT_LIB_SRCS
	omProject.cpp
)


#create static library
ADD_LIBRARY(omProject 
	${OM_PROJECT_LIB_SRCS}
)



TARGET_LINK_LIBRARIES( omProject
	omCommon
	omVolume
	omSystem
)




# EXECUTABLE
#specify executable
ADD_EXECUTABLE( omProjectApp
	main.cpp
)

#link binaries targets against libraries
TARGET_LINK_LIBRARIES( omProjectApp 
	omProject
)


