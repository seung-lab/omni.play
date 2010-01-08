

## LIBRARY ##

#specify lib sources
SET(OM_LIB_SRCS
	strnatcmp.cpp
)



#create static library
ADD_LIBRARY(omLibrary
	${OM_LIB_SRCS} 
)







## EXECUTABLE ##

#specify executable
#ADD_EXECUTABLE( omSystemApp
#	main.cpp
#)


#TARGET_LINK_LIBRARIES( omSystemApp
#	omSystem
#)
