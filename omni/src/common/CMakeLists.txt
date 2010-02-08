

#specify include files
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
)



#specify sources
SET(OM_COMMON_SRCS
	omGl.cpp
	omStd.cpp
)


#create static library
ADD_LIBRARY(omCommon
	${OM_COMMON_SRCS}
)


TARGET_LINK_LIBRARIES( omCommon
	${OM_VTK_LIBS}
	${OM_BOOST_LIBS}

	${GLUT_LIBRARY} 
	${OPENGL_LIBRARIES}
)
