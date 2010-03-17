#------------------------------------------------------------
# This is the list of source files that need to be compiled
#------------------------------------------------------------
SET( OMGUI_SRCS
	main.cpp
	headless.cpp
	mainwindow.cpp
	recentFileList.cpp

	../utility/dataWrappers.cpp
	../system/omAnimate.cpp
	guiUtils.cpp
	segmentList.cpp
	elementListBox.cpp

	inspectors/segInspector.cpp
	inspectors/chanInspector.cpp
	inspectors/segObjectInspector.cpp
	inspectors/filObjectInspector.cpp
	inspectors/inspectorProperties.cpp
	inspectors/mutexServer.cpp

        preferences/preferencesMesh.cpp
        preferences/preferences3d.cpp
	preferences/preferences2d.cpp
	preferences/preferences.cpp
	preferences/localPreferencesSystem.cpp
	preferences/localPreferencesMeshing.cpp
	preferences/localPreferences2d.cpp
	preferences/localPreferences3d.cpp
	preferences/volumeInspector.cpp

	myInspectorWidget.cpp
)


#--------------------------------------------------------------------
# another list, this time it includes all header files that should 
#  be treated with moc
#-------------------------------------------------------------------
SET( OMGUI_MOC_HDRS
	mainwindow.h
	../view3d/omView3d.h
	../view2d/omView2d.h
	segmentList.h
	elementListBox.h

	inspectors/segInspector.h
	inspectors/chanInspector.h
	inspectors/segObjectInspector.h
	inspectors/filObjectInspector.h
	inspectors/inspectorProperties.h
	inspectors/mutexServer.h

	preferences/preferencesMesh.h
	preferences/preferences3d.h
	preferences/preferences2d.h
	preferences/preferences.h
	preferences/localPreferencesSystem.h
	preferences/localPreferencesMeshing.h
	preferences/localPreferences2d.h
	preferences/localPreferences3d.h
	preferences/volumeInspector.h

	myInspectorWidget.h
)

#-------------------------------------------------------------------
# .ui files that are used in the project
#-------------------------------------------------------------------
SET( OMGUI_UIS

)

#-------------------------------------------------------------------
# and finally any resource files.
#-------------------------------------------------------------------
SET( OMGUI_RCS
	resources.qrc
)

#-------------------------------------------------------------------
# this command will generate rules that will run rcc on all files from OMGUI_RCS
# in result OMGUI_GEN_RCS variable will contain paths to files produced by rcc
QT4_ADD_RESOURCES( OMGUI_GEN_RCS ${OMGUI_RCS} )

#-------------------------------------------------------------------
# this will run uic on .ui files:
QT4_WRAP_UI( OMGUI_GEN_UIS ${OMGUI_UIS} )

#-------------------------------------------------------------------
# and finally this will run moc on the headers
QT4_WRAP_CPP( OMGUI_GEN_MOC_HDRS ${OMGUI_MOC_HDRS} )

#-------------------------------------------------------------------
# we need this to be able to include headers produced by uic in our code
# CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES()
#  works just like INCLUDEPATH from qmake
INCLUDE_DIRECTORIES (
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR} # Look in the cmake build directory
  ${CMAKE_CURRENT_SOURCE_DIR} # Look in the current source directory
)

#-------------------------------------------------------------------
# Sets a variable that signals CMake with a keyword appropriate for each platform
#  that will create the proper executable.
SET(GUI_TYPE)
IF(APPLE)
	SET(GUI_TYPE MACOSX_BUNDLE)
	SET(OMGUI_SRCS ${OMGUI_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Omni.icns)
	SET(MACOSX_BUNDLE_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Omni.icns)
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/Resources/Omni.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

ENDIF (APPLE)



#-------------------------------------------------------------------
#  CMake's way of creating an executable
ADD_EXECUTABLE( omni ${GUI_TYPE}
  ${OMGUI_SRCS}
  ${OMGUI_GEN_MOC_HDRS} 
  ${OMGUI_GEN_UIS}
  ${OMGUI_GEN_RCS}
)

#-------------------------------------------------------------------
# Tell CMake which libraries we need to link our executable against.
TARGET_LINK_LIBRARIES(omni
	omCommon
	omVolume
	omSystem
	omSegment
	omView3d
	omView2d
	omProject
	${OM_QT_LIBS}
)
