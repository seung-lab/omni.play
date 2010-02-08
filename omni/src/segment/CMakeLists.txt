

## LIBRARY ##



SET(OM_EDIT_ACTIONS_SRCS
	actions/edit/omEditSelectionSetAction.cpp
)

SET(OM_SEGMENT_ACTIONS_SRCS
	actions/segment/omSegmentMergeAction.cpp
	actions/segment/omSegmentSelectAction.cpp
	actions/segment/omSegmentStateAction.cpp
)

SET(OM_VOXEL_ACTIONS_SRCS
	actions/voxel/omVoxelSelectionAction.cpp
	actions/voxel/omVoxelSetAction.cpp
	actions/voxel/omVoxelSetValueAction.cpp
	actions/voxel/omVoxelSetConnectedAction.cpp

)

SET(OM_ACTIONS_SRCS
	${OM_EDIT_ACTIONS_SRCS}
	${OM_SEGMENT_ACTIONS_SRCS}
	${OM_VOXEL_ACTIONS_SRCS}
)
	


#specify sources
SET(OM_SEGMENT_LIB_SRCS
	omSegment.cpp
	omSegmentContextMenu.cpp
	omSegmentDataMap.cpp
	omSegmentEditor.cpp
	omSegmentManager.cpp
	
	${OM_ACTIONS_SRCS}
)




#qt moc wrapping
SET(OM_SEGMENT_MOC_HDRS
	omSegmentContextMenu.h
)

QT4_WRAP_CPP(OM_SEGMENT_MOC_SRCS
	${OM_SEGMENT_MOC_HDRS}
)



#create static library
ADD_LIBRARY(omSegment 
	${OM_SEGMENT_LIB_SRCS}
	${OM_SEGMENT_MOC_SRCS}
)


TARGET_LINK_LIBRARIES( omSegment
	omMesh
	omSystem
	omVolume
)





## EXECUTABLE ##

#specify executable
#ADD_EXECUTABLE(omSegmentApp
#	main.cpp
#)


#TARGET_LINK_LIBRARIES( omSegmentApp
#	omSegment
#)
