
cmake_minimum_required(VERSION 2.6)


#### project properties ####
PROJECT(OMNI)

SET(OM_VERSION_MAJOR 0)
SET(OM_VERSION_MINOR 2)
SET(OM_VERSION_PATCH 0)


MESSAGE("Omni ${OM_VERSION_MAJOR}.${OM_VERSION_MINOR}.${OM_VERSION_PATCH}")
MESSAGE("Build Script 5/8/09")


#### external library directory ####
SET(OM_EXT_LIBS_DIR "/Users/mwimer/omni1/external/libs")




#### external library locations ####
MESSAGE("Using external library directory: ${OM_EXT_LIBS_DIR}")


#set lib versions
SET(VTK_VERSION "5.4")
#SET(BOOST_VERS_EXT "xgcc40-mt-1_37")
#SET(BOOST_VERS_EXT "xgcc40-mt-1_37")
SET(BOOST_VERS_EXT "gcc43-mt-1_38")



#set library dirs
#QTDIR and BOOST_ROOT set in .bashrc file - see README.txt

SET(VTK_DIR "${OM_EXT_LIBS_DIR}/VTK/lib/vtk-${VTK_VERSION}")
SET(HDF5_DIR "${OM_EXT_LIBS_DIR}/HDF5")
SET(FREETYPE_DIR "${OM_EXT_LIBS_DIR}/FreeType")
SET(FONTCONFIG_DIR "${OM_EXT_LIBS_DIR}/Fontconfig")
SET(PNG_DIR "${OM_EXT_LIBS_DIR}/libpng")






#### build preferences ####

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/lib
)


SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin" 
	CACHE INTERNAL "set executable location" )


SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules")





#### attach packages ####

# attach Qt4
FIND_PACKAGE(Qt)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE}) 

# attach VTK
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# attach Boost
FIND_PACKAGE(Boost)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )


# attach hdf5
FIND_PACKAGE(HDF5)
INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})

# attach OpenGL
INCLUDE(FindOpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

# attach pthreads
INCLUDE(FindThreads REQUIRED)

# attach image libraries
INCLUDE(FindPNG REQUIRED)






#### compatability ####

#specify vtk macro
SET(OM_VTK_LIBS
	vtkHybrid
)

SET(OM_BOOST_LIBS
	$ENV{BOOST_ROOT}/lib/libboost_system-${BOOST_VERS_EXT}.a
	$ENV{BOOST_ROOT}/lib/libboost_filesystem-${BOOST_VERS_EXT}.a
	$ENV{BOOST_ROOT}/lib/libboost_regex-${BOOST_VERS_EXT}.a
	$ENV{BOOST_ROOT}/lib/libboost_serialization-${BOOST_VERS_EXT}.a
)


# libraries for OSX
if( APPLE )
	INCLUDE(FindZLIB REQUIRED)
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )

	SET(OM_QT_MAC_LIBS
		${ZLIB_LIBRARIES}
		${PNG_DIR}/lib/libpng.a
			
		${CARBON_LIBRARY}
		${APP_SERVICES_LIBRARY}
		${OPENGL_LIBRARIES}

		#${QT_LIBRARIES}
		$ENV{QTDIR}/lib/libQtCore.a
		$ENV{QTDIR}/lib/libQtOpenGL.a
		$ENV{QTDIR}/lib/libQtGui.a	

		${FONTCONFIG_DIR}/lib/libfontconfig.a
		${FREETYPE_DIR}/lib/libfreetype.a
	)
	
	SET(OM_QT_LIBS ${OM_QT_MAC_LIBS})
	
endif( APPLE )



# libraries for LINUX
if( UNIX AND NOT APPLE )
	
	INCLUDE(FindZLIB REQUIRED)
	INCLUDE(FindX11 REQUIRED)
	INCLUDE(FindEXPAT REQUIRED)

	SET(OM_QT_LINUX_LIBS
		/usr/lib/librt.so
		${ZLIB_LIBRARIES}
		${PNG_LIBRARIES}
		${EXPAT_LIBRARIES}

		$ENV{QTDIR}/lib/libQtOpenGL.a
		${OPENGL_LIBRARIES}

		$ENV{QTDIR}/lib/libQtGui.a
		${X11_Xrender_LIB}
		${X11_LIBRARIES}
		/usr/lib/libm.so

		$ENV{QTDIR}/lib/libQtCore.a
		-lpthread
		-ldl	

		${FONTCONFIG_DIR}/lib/libfontconfig.a
		${FREETYPE_DIR}/lib/libfreetype.a
	)
	
	message(${QT_LIBRARIES})
	SET(OM_QT_LIBS ${OM_QT_LINUX_LIBS})

endif( UNIX AND NOT APPLE )




#MESSAGE(${OM_BOOST_LIBS})
#MESSAGE(${OM_QT_LIBS})







#### subdirectories ####
#add src and lib dirs, send intermediate files to inst
ADD_SUBDIRECTORY(src inst)
ADD_SUBDIRECTORY(lib inst.lib)



# http://www.cmake.org/Wiki/CMake_Useful_Variables

