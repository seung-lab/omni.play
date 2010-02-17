
set(QT_USE_QTOPENGL TRUE)


#specify include files
#INCLUDE_DIRECTORIES(
#		${PROJECT_SOURCE_DIR}/include
#		${PROJECT_SOURCE_DIR}/src
#)

# specify sources
# set(OM_GUI_LIB_SRCS
#    main.cpp
#    mainwindow.cpp
# )

# tell cmake to create .moc files for all files in
# the variable OM_GUI_LIB_SRCS that require such a
# file. note: this assumes that you use
# <<#include "header.moc">> in your files

SET(omGuiApp_MOC_HDRS
	mainwindow.h
	../view3d/omView3d.h
	../view2d/omView2d.h
	inspectors/segInspector.h
	inspectors/chanInspector.h
	inspectors/segObjectInspector.h
	inspectors/filObjectInspector.h
	inspectorProperties.h
	preferences/preferencesMesh.h
	preferences/preferences3d.h
	preferences/preferences2d.h
	preferences/preferences.h
	preferences/volumeInspector.h
	myInspectorWidget.h
)

SET(omGuiApp_RCCS resources.qrc)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(omGuiApp_RCC_SRCS ${omGuiApp_RCCS})


QT4_WRAP_CPP(omGuiApp_MOC_SRCS ${omGuiApp_MOC_HDRS})


# create an executable file named "qtproject" from
# the source files in the variable "qtproject_SRCS".
ADD_EXECUTABLE(omni
	main.cpp
	mainwindow.cpp
	recentFileList.cpp
	../utility/dataWrappers.cpp
	../system/omAnimate.cpp
	inspectors/segInspector.cpp
	inspectors/chanInspector.cpp
	inspectors/segObjectInspector.cpp
	inspectors/filObjectInspector.cpp
	inspectorProperties.cpp
        preferences/preferencesMesh.cpp
        preferences/preferences3d.cpp
	preferences/preferences2d.cpp
	preferences/preferences.cpp
	preferences/volumeInspector.cpp
	myInspectorWidget.cpp
	${omGuiApp_MOC_SRCS}
	${omGuiApp_RCC_SRCS}
)

#create static library
#ADD_LIBRARY(omSystem 
#	${OM_GUI_LIB_SRCS}
#)



#link binaries targets against libraries
TARGET_LINK_LIBRARIES(omni

	omCommon
	omVolume
	omSystem
	omSegment
	omView3d
	omView2d
	${OM_QT_LIBS}
)
