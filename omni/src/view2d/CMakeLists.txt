
# LIBRARY

#specify sources
SET(OM_VIEW2D_SRCS
	omTile.cpp
	omTextureID.cpp
	omThreadedCachingTile.cpp
	omView2d.cpp
	omView2dEvent.cpp
	omView2dConverters.cpp
	omTileCoord.cpp
	drawable.cpp
)

#create static library
ADD_LIBRARY(omView2d 
	${OM_VIEW2D_SRCS}
)

TARGET_LINK_LIBRARIES( omView2d
	omCommon
	omSystem
	omVolume
	omSegment
)

# EXECUTABLE

SET(OM_VIEW2D_MOC_HDRS
  mainwindow.h
  omView2d.h
)
QT4_WRAP_CPP(OM_VIEW2D_MOC_SRCS 
				${OM_VIEW2D_MOC_HDRS})

#specify executable
#ADD_EXECUTABLE(omView2dApp
#	main.cpp
#	mainwindow.cpp 
#	${OM_VIEW2D_MOC_SRCS})

#link binaries targets against libraries
#TARGET_LINK_LIBRARIES(
#	omView2dApp 
#	omView2d
#)
