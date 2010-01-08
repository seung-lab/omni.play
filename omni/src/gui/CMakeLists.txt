
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
  glview.h
../view3d/omView3d.h
../view2d/omView2d.h
../view2d/domView2d.h

# ui_segInspector.h
  segInspector.h

#  ui_volumeInspector.h
  volumeInspector.h

#  ui_chanInspector.h
  chanInspector.h

#  ui_segObjectInspector.h
  segObjectInspector.h

  filObjectInspector.h

  preferencesMesh.h

  preferences3d.h

  preferences2d.h

  myInspectorWidget.h
)

SET(omGuiApp_RCCS resources.qrc)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(omGuiApp_RCC_SRCS ${omGuiApp_RCCS})


QT4_WRAP_CPP(omGuiApp_MOC_SRCS ${omGuiApp_MOC_HDRS})


# create an executable file named "qtproject" from
# the source files in the variable "qtproject_SRCS".
ADD_EXECUTABLE(omGuiApp 
	main.cpp
	mainwindow.cpp
	recentFileList.cpp
	dataWrappers.cpp
	glview.cpp
	segInspector.cpp
	volumeInspector.cpp
	chanInspector.cpp
	segObjectInspector.cpp
	filObjectInspector.cpp
        preferencesMesh.cpp
        preferences3d.cpp
	preferences2d.cpp
	myInspectorWidget.cpp
	${omGuiApp_MOC_SRCS}
	${omGuiApp_RCC_SRCS}
)

#create static library
#ADD_LIBRARY(omSystem 
#	${OM_GUI_LIB_SRCS}
#)



#link binaries targets against libraries
TARGET_LINK_LIBRARIES(omGuiApp

	omCommon
	omVolume
	omSystem
	omSegment
	omView3d
	omView2d
	domView2d
	${OM_QT_LIBS}
)
