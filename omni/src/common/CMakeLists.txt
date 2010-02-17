

#specify sources
SET(OM_COMMON_SRCS
	omDebug.cpp
	omException.cpp
	omGl.cpp
	omStd.cpp
)


#create static library
ADD_LIBRARY(omCommon
	${OM_COMMON_SRCS}
)


TARGET_LINK_LIBRARIES( omCommon
	${GLUT_LIBRARY} 
	${OPENGL_LIBRARIES}
)
