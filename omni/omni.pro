QT += opengl network
TEMPLATE = app

# headers that must be processed by QT moc (i.e. contain Q_OBJECT)
HEADERS += src/gui/cacheMonitorDialog.h  \
           src/gui/cacheMonitorWidget.h \ \
           src/gui/elementListBox.h \
           src/gui/inspectors/chanInspector.h \
           src/gui/inspectors/filObjectInspector.h \
           src/gui/inspectors/inspectorProperties.h \
           src/gui/inspectors/segInspector.h \
           src/gui/inspectors/segObjectInspector.h \
           src/gui/inspectors/volInspector.h \
           src/gui/mainwindow.h \
           src/gui/menubar.h \
           src/gui/myInspectorWidget.h \
           src/gui/omSegmentContextMenu.h \
           src/gui/preferences/localPreferences2d.h \
           src/gui/preferences/localPreferences3d.h \
           src/gui/preferences/localPreferencesMeshing.h \
           src/gui/preferences/localPreferencesSystem.h \
           src/gui/preferences/preferences2d.h \
           src/gui/preferences/preferences3d.h \
           src/gui/preferences/preferences.h \
           src/gui/preferences/preferencesMesh.h \
           src/gui/segmentListBase.h \
           src/gui/segmentListRecent.h \
           src/gui/segmentListValid.h \
           src/gui/segmentListWorking.h \
           src/gui/toolbars/dendToolbar/dendToolbar.h \
           src/gui/toolbars/mainToolbar/filterWidget.h \
           src/gui/toolbars/mainToolbar/mainToolbar.h \
           src/gui/toolbars/mainToolbar/navAndEditButtonGroup.h \
           src/gui/toolbars/toolbarManager.h \
           src/gui/widgets/omSegmentListWidget.h \
           src/view2d/omView2d.h \
           src/view2d/omView2dImpl.h \
           src/view3d/omView3d.h

SOURCES += lib/strnatcmp.cpp \
           src/common/omCommon.cpp \
           src/common/omDebug.cpp \
           src/common/omException.cpp \
           src/common/omGl.cpp \
           src/common/omStd.cpp \
           src/datalayer/archive/omDataArchiveBoost.cpp \
           src/datalayer/archive/omDataArchiveCoords.cpp \
           src/datalayer/archive/omDataArchiveMipChunk.cpp \
           src/datalayer/archive/omDataArchiveProject.cpp \
           src/datalayer/archive/omDataArchiveQT.cpp \
           src/datalayer/archive/omDataArchiveSegment.cpp \
           src/datalayer/archive/omDataArchiveVmml.cpp \
           src/datalayer/hdf5/omHdf5.cpp \
           src/datalayer/hdf5/omHdf5LowLevel.cpp \
           src/datalayer/hdf5/omHdf5LowLevelWrappersManualOpenClose.cpp \
           src/datalayer/hdf5/omHdf5Manager.cpp \
           src/datalayer/hdf5/omHdf5Reader.cpp \
           src/datalayer/hdf5/omHdf5Writer.cpp \
           src/datalayer/omDataLayer.cpp \
           src/datalayer/omDataPath.cpp \
           src/datalayer/omDataPaths.cpp \
           src/datalayer/omDummyWriter.cpp \
           src/gui/cacheMonitorDialog.cpp \
           src/gui/cacheMonitorWidget.cpp \
           src/gui/elementListBox.cpp \
           src/gui/guiUtils.cpp \
           src/gui/headless.cpp \
           src/gui/inspectors/chanInspector.cpp \
           src/gui/inspectors/filObjectInspector.cpp \
           src/gui/inspectors/inspectorProperties.cpp \
           src/gui/inspectors/segInspector.cpp \
           src/gui/inspectors/segObjectInspector.cpp \
           src/gui/inspectors/volInspector.cpp \
           src/gui/main.cpp \
           src/gui/mainwindow.cpp \
           src/gui/menubar.cpp \
           src/gui/myInspectorWidget.cpp \
           src/gui/omSegmentContextMenu.cpp \
           src/gui/preferences/localPreferences2d.cpp \
           src/gui/preferences/localPreferences3d.cpp \
           src/gui/preferences/localPreferencesMeshing.cpp \
           src/gui/preferences/localPreferencesSystem.cpp \
           src/gui/preferences/preferences.cpp \
           src/gui/preferences/preferences2d.cpp \
           src/gui/preferences/preferences3d.cpp \
           src/gui/preferences/preferencesMesh.cpp \
           src/gui/recentFileList.cpp \
           src/gui/segmentListBase.cpp \
           src/gui/segmentListRecent.cpp \
           src/gui/segmentListValid.cpp \
           src/gui/segmentListWorking.cpp \
           src/gui/toolbars/dendToolbar/dendToolbar.cpp \
           src/gui/toolbars/dendToolbar/splitButton.cpp \
           src/gui/toolbars/mainToolbar/filterWidget.cpp \
           src/gui/toolbars/mainToolbar/mainToolbar.cpp \
           src/gui/toolbars/mainToolbar/navAndEditButtonGroup.cpp \
           src/gui/toolbars/mainToolbar/navButtons.cpp \
           src/gui/toolbars/mainToolbar/openViewGroupButton.cpp \
           src/gui/toolbars/mainToolbar/saveButton.cpp \
           src/gui/toolbars/mainToolbar/toolButton.cpp \
           src/gui/toolbars/toolbarManager.cpp \
           src/gui/viewGroup.cpp \
           src/gui/widgets/omButton.cpp \
           src/gui/widgets/omSegmentListWidget.cpp \
           src/mesh/meshingChunkThread.cpp \
           src/mesh/meshingChunkThreadManager.cpp \
           src/mesh/meshingManager.cpp \
           src/mesh/omMeshDrawer.cpp \
           src/mesh/omMeshSource.cpp \
           src/mesh/omMesher.cpp \
           src/mesh/omMipMesh.cpp \
           src/mesh/omMipMeshCoord.cpp \
           src/mesh/omMipMeshManager.cpp \
           src/project/omProject.cpp \
           src/segment/actions/omSegmentEditor.cpp \
           src/segment/actions/segment/omSegmentJoinAction.cpp \
           src/segment/actions/segment/omSegmentSelectAction.cpp \
           src/segment/actions/segment/omSegmentSplitAction.cpp \
           src/segment/actions/voxel/omVoxelSelectionAction.cpp \
           src/segment/actions/voxel/omVoxelSetAction.cpp \
           src/segment/actions/voxel/omVoxelSetConnectedAction.cpp \
           src/segment/actions/voxel/omVoxelSetValueAction.cpp \
           src/segment/lowLevel/DynamicTree.cpp \
           src/segment/lowLevel/DynamicTreeContainer.cpp \
           src/segment/lowLevel/omPagingStore.cpp \
           src/segment/lowLevel/omSegmentCacheImplLowLevel.cpp \
           src/segment/lowLevel/omSegmentGraph.cpp \
           src/segment/lowLevel/omSegmentIteratorLowLevel.cpp \
           src/segment/lowLevel/omSegmentListByMRU.cpp \
           src/segment/lowLevel/omSegmentListBySize.cpp \
           src/segment/omSegment.cpp \
           src/segment/omSegmentCache.cpp \
           src/segment/omSegmentCacheImpl.cpp \
           src/segment/omSegmentColorizer.cpp \
           src/segment/omSegmentEdge.cpp \
           src/segment/omSegmentIterator.cpp \
           src/segment/omSegmentQueue.cpp \
           src/segment/omSegmentSelector.cpp \
           src/system/OmCacheInfo.cpp \
           src/system/events/omPreferenceEvent.cpp \
           src/system/events/omProgressEvent.cpp \
           src/system/events/omSegmentEvent.cpp \
           src/system/events/omToolModeEvent.cpp \
           src/system/events/omView3dEvent.cpp \
           src/system/events/omViewEvent.cpp \
           src/system/events/omVoxelEvent.cpp \
           src/system/omAction.cpp \
           src/system/omAnimate.cpp \
           src/system/omBuildChannel.cpp \
           src/system/omBuildSegmentation.cpp \
           src/system/omBuildVolumes.cpp \
           src/system/omCacheManager.cpp \
           src/system/omCacheableBase.cpp \
           src/system/omEvent.cpp \
           src/system/omEventManager.cpp \
           src/system/omEvents.cpp \
           src/system/omFetchingThread.cpp \
           src/system/omGarbage.cpp \
           src/system/omGenericManager.cpp \
           src/system/omGroup.cpp \
           src/system/omGroups.cpp \
           src/system/omLocalPreferences.cpp \
           src/system/omManageableObject.cpp \
           src/system/omPreferenceDefinitions.cpp \
           src/system/omPreferences.cpp \
           src/system/omProjectData.cpp \
           src/system/omStateManager.cpp \
           src/system/omThreadedCache.cpp \
           src/system/templatedClasses.cpp \
           src/system/viewGroup/omViewGroupState.cpp \
           src/utility/dataWrappers.cpp \
           src/utility/fileHelpers.cpp \
           src/utility/localPrefFiles.cpp \
           src/utility/omImageDataIo.cpp \
           src/utility/omSystemInformation.cpp \
           src/utility/sortHelpers.cpp \
           src/utility/stringHelpers.cpp \
           src/view2d/drawable.cpp \
           src/view2d/omTextureID.cpp \
           src/view2d/omThreadedCachingTile.cpp \
           src/view2d/omTile.cpp \
           src/view2d/omTileCoord.cpp \
           src/view2d/omView2d.cpp \
           src/view2d/omView2dConverters.cpp \
           src/view2d/omView2dEvent.cpp \
           src/view2d/omView2dImpl.cpp \
           src/view3d/omCamera.cpp \
           src/view3d/omCameraMovement.cpp \
           src/view3d/omView3d.cpp \
           src/view3d/omView3dUi.cpp \
           src/view3d/widgets/omChunkExtentWidget.cpp \
           src/view3d/widgets/omInfoWidget.cpp \
           src/view3d/widgets/omSelectionWidget.cpp \
           src/view3d/widgets/omViewBoxWidget.cpp \
           src/volume/omChannel.cpp \
           src/volume/omFilter2d.cpp \
           src/volume/omFilter2dManager.cpp \
           src/volume/omMipChunk.cpp \
           src/volume/omMipChunkCoord.cpp \
           src/volume/omMipVolume.cpp \
           src/volume/omSegmentation.cpp \
           src/volume/omSegmentationChunkCoord.cpp \
           src/volume/omSimpleChunk.cpp \
           src/volume/omSimpleChunkThreadedCache.cpp \
           src/volume/omVolume.cpp \
           src/volume/omVolumeCuller.cpp \
           src/voxel/omMipSegmentDataCoord.cpp \
           src/voxel/omMipVoxelation.cpp \
           src/voxel/omMipVoxelationManager.cpp \
           tests/utility/stringHelpersTest.cpp

RESOURCES += src/gui/resources.qrc

INCLUDEPATH = src include lib

LIBS += -lvtkHybrid -lvtkRendering -lvtkGraphics -lvtkverdict -lvtkImaging -lvtkIO -lvtkFiltering -lvtkCommon -lvtkDICOMParser -lvtkmetaio -lvtksqlite -lvtkpng -lvtktiff -lvtkzlib -lvtkjpeg -lvtkexpat -lvtksys -lvtkexoIIc -lvtkNetCDF 

win32 {

INCLUDEPATH += c:/hdf5lib/include c:/dev/external/libs/VTK/include/vtk-5.4/ c:/dev/external/libs/libtiff/include C:/mygl C:/omni/external/libs/VTK/include/vtk-5.4  C:/mygl C:/omni/external/libs/libtiff/include

LIBS += /omni/external/srcs/hdf5-1.8.4-patch1/src/.libs/libhdf5.a  -L/drivec/omni/external/libs/VTK/lib/vtk-5.4/  
LIBS += -lgdi32 

} else {

INCLUDEPATH +=  ../external/libs/HDF5/include ../external/libs/VTK/include/vtk-5.4/ ../external/libs/libtiff/include

INCLUDEPATH += ../external/headers/boost_1_43_0

LIBS += ../external/libs/HDF5/lib/libhdf5.a  -L../external/libs/VTK/lib/vtk-5.4/  
LIBS += -lz
}

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
DESTDIR = bin

#### for static build
#CONFIG += qt warn_on static
#QMAKE_LFLAGS += -static

#### for profiling
#QMAKE_CXXFLAGS += -pg
#QMAKE_LFLAGS   += -pg
