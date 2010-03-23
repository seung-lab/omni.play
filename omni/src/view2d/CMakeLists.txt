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
