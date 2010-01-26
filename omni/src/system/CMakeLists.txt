

## LIBRARY ##

#specify events sources
SET(OM_SYSTEM_EVENTS_SRCS
	events/omPreferenceEvent.cpp
	events/omProgressEvent.cpp
	events/omSegmentEvent.cpp
	events/omSystemModeEvent.cpp
	events/omToolModeEvent.cpp
	events/omViewEvent.cpp
	events/omView3dEvent.cpp
	events/omVoxelEvent.cpp
	events/omAlertEvent.cpp
)


#specify sources
SET(OM_SYSTEM_LIB_SRCS
	omAction.cpp
	omCacheManager.cpp
	omEvent.cpp
	omEventManager.cpp
	omException.cpp
	omKeyManager.cpp
	omPreferenceDefinitions.cpp
	omPreferences.cpp
	omProject.cpp
	omProjectData.cpp
	omStateManager.cpp
	omTagManager.cpp
	omFilter.cpp
	omFilterManager.cpp
	omGarbage.cpp
	omDebug.cpp
#	omSnapshot.cpp
	
	${OM_SYSTEM_EVENTS_SRCS}
)


#create static library
ADD_LIBRARY(omSystem
	${OM_SYSTEM_LIB_SRCS}
)


TARGET_LINK_LIBRARIES(omSystem
	omCommon
	omUtility
	omVolume
	-lpthread
	${OM_QT_LIBS}
)







## EXECUTABLE ##

#specify executable
#ADD_EXECUTABLE( omSystemApp
#	main.cpp
#)
#TARGET_LINK_LIBRARIES( omSystemApp
#	omSystem
#)
