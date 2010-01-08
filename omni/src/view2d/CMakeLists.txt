
# LIBRARY

#specify sources
SET(OM_VIEW2D_SRCS
	omTile.cpp
	omTextureID.cpp
	omCachingTile.cpp
	omThreadedCachingTile.cpp
	omView2d.cpp
	omTileCoord.cpp
)

#specify sources
SET(DOM_VIEW2D_SRCS
	omTile.cpp
	omTextureID.cpp
	omCachingTile.cpp
	omThreadedCachingTile.cpp
	domView2d.cpp
	omTileCoord.cpp
)


#create static library
ADD_LIBRARY(omView2d 
	${OM_VIEW2D_SRCS}
)

#create static library
ADD_LIBRARY(domView2d 
	${DOM_VIEW2D_SRCS}
)

TARGET_LINK_LIBRARIES( omView2d
	omCommon
	omSystem
	omVolume
	omSegment
)


TARGET_LINK_LIBRARIES( domView2d
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
SET(DOM_VIEW2D_MOC_HDRS
  dmainwindow.h
  domView2d.h
)

QT4_WRAP_CPP(OM_VIEW2D_MOC_SRCS 
				${OM_VIEW2D_MOC_HDRS})

QT4_WRAP_CPP(DOM_VIEW2D_MOC_SRCS 
				${DOM_VIEW2D_MOC_HDRS})

#specify executable
ADD_EXECUTABLE(omView2dApp
	main.cpp
	mainwindow.cpp 
	${OM_VIEW2D_MOC_SRCS})

#specify executable
ADD_EXECUTABLE(domView2dApp
	main.cpp
	dmainwindow.cpp 
	${DOM_VIEW2D_MOC_SRCS})

#link binaries targets against libraries
TARGET_LINK_LIBRARIES(
	omView2dApp 
	omView2d
)

#link binaries targets against libraries
TARGET_LINK_LIBRARIES(
	domView2dApp 
	domView2d
)
