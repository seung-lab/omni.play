



## LIBRARY ##

# widget sources
SET(OM_VIEW3D_WIDGET_SRCS
	widgets/omChunkExtentWidget.cpp
	widgets/omInfoWidget.cpp
	widgets/omSelectionWidget.cpp
	widgets/omViewBoxWidget.cpp
)


# library sources
set( OM_VIEW3D_SRCS
	omCamera.cpp
	omCameraMovement.cpp
	omView3d.cpp
	omView3dUi.cpp
	
	${OM_VIEW3D_WIDGET_SRCS}
)


#create static library
ADD_LIBRARY( omView3d 
	${OM_VIEW3D_SRCS}
)

TARGET_LINK_LIBRARIES( omView3d
	omCommon
	omSystem
	omVolume
	omSegment
)








## EXECUTABLE ##

# tell cmake to create .moc files for all files in
# the variable qtproject_SRCS that require such a
# file. note: this assumes that you use
# <<#include "header.moc">> in your files

SET(OM_VIEW3D_MOC_HDRS
  mainwindow.h
  omView3d.h
)

# After this call, foo_MOC_SRCS =
#    moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(OM_VIEW3D_MOC_SRCS 
				${OM_VIEW3D_MOC_HDRS})


# create an executable file named "qtproject" from
# the source files in the variable "qtproject_SRCS".
# add_executable(qtproject ${qtproject_SRCS})
#ADD_EXECUTABLE(omView3dApp 
#	main.cpp 
#	mainwindow.cpp 
#	${OM_VIEW3D_MOC_SRCS})

# link the "qtproject" target against the Qt
# libraries. which libraries exactly, is defined by
# the "include(${QT_USE_FILE})" line above, which
# sets up this variable.
#target_link_libraries(omView3dApp
#	omView3d 
#)
