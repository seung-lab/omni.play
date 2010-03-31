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
