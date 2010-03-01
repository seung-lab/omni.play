

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
	omKeyManager.cpp
	omPreferenceDefinitions.cpp
	omPreferences.cpp
	omProjectData.cpp
	omStateManager.cpp
	omTagManager.cpp
	omGarbage.cpp
#	omViewList.cpp
#	omSnapshot.cpp
	omLocalPreferences.cpp	
	${OM_SYSTEM_EVENTS_SRCS}
)


#create static library
ADD_LIBRARY(omSystem
	${OM_SYSTEM_LIB_SRCS}
)


TARGET_LINK_LIBRARIES(omSystem
	omCommon
	omUtility
	${OM_QT_LIBS}
	-lpthread
)

## EXECUTABLE ##

#specify executable
#ADD_EXECUTABLE( omSystemApp
#	main.cpp
#)
#TARGET_LINK_LIBRARIES( omSystemApp
#	omSystem
#)
