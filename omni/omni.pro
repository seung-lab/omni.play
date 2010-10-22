QT += opengl
TEMPLATE = app
CONFIG += qt

mac {
  CONFIG += release
}
else {
 CONFIG += debug
}

DEPENDPATH += . \
           include \
           include/vmmlib \
           include/enum \
           include/enum/enum \
           lib \
           src \
           src/common \
           src/datalayer \
           src/datalayer/archive \
           src/datalayer/hdf5 \
           src/datalayer/upgraders \
           src/gui \
           src/gui/images \
           src/gui/inspectors \
           src/gui/preferences \
           src/gui/toolbars \
           src/gui/toolbars/dendToolbar \
           src/gui/toolbars/mainToolbar \
           src/gui/segmentLists \
           src/gui/segmentLists/details \
           src/gui/widgets \
           src/mesh \
           src/project \
           src/segment \
           src/actions \
           src/segment/lowLevel \
           src/system \
           src/system/events \
           src/utility \
           src/utility/bin \
           src/utility/build \
           src/view2d \
           src/view3d \
           src/view3d/widgets \
           src/viewGroup \
           src/volume \
           tests \
           tests/segment \
           tests/segment/helpers \
           tests/utility

HEADERS += lib/strnatcmp.h \
           src/common/omCommon.h \
           src/common/omDebug.h \
           src/common/omException.h \
           src/common/omGl.h \
           src/common/omStd.h \
           src/datalayer/fs/omIOnDiskFile.h \
           src/datalayer/fs/omFileQT.hpp \
           src/datalayer/fs/omMemMappedFileQT.hpp \
           src/datalayer/fs/omFileNames.hpp \
           src/datalayer/omIDataVolume.hpp \
           src/segment/io/omMST.h \
           src/segment/io/omMSTold.h \
           src/datalayer/archive/omDataArchiveBoost.h \
           src/datalayer/archive/omDataArchiveCoords.h \
           src/datalayer/archive/omDataArchiveMipChunk.h \
           src/datalayer/archive/omDataArchiveProject.h \
           src/datalayer/archive/omDataArchiveSegment.h \
           src/datalayer/upgraders/omUpgraders.hpp \
           src/datalayer/upgraders/omUpgradeTo14.hpp \
           src/datalayer/fs/omActionLoggerFS.h \
           src/datalayer/fs/omActionLoggerFSthread.hpp \
           src/datalayer/fs/omMemMappedVolume.hpp \
           src/datalayer/hdf5/omHdf5.h \
           src/datalayer/hdf5/omHdf5FileUtils.hpp \
           src/datalayer/hdf5/omHdf5Utils.hpp \
           src/datalayer/hdf5/omHdf5LowLevel.h \
           src/datalayer/hdf5/omHdf5Impl.h \
           src/datalayer/hdf5/omHdf5Manager.h \
           src/datalayer/omDataLayer.h \
           src/datalayer/omDataPath.h \
           src/datalayer/omDataPaths.h \
           src/datalayer/omIDataReader.h \
           src/datalayer/omDataWrapper.h \
           src/datalayer/omIDataWriter.h \
           src/datalayer/omDummyWriter.h \
           src/gui/meshPreviewer/meshPreviewer.hpp \
           src/gui/mstViewer.hpp \
           src/gui/cacheMonitorDialog.h \
           src/gui/groupsTable.h \
           src/gui/cacheMonitorWidget.h \
           src/gui/segmentLists/elementListBox.hpp \
           src/gui/segmentLists/elementListBoxImpl.hpp \
           src/gui/guiUtils.h \
           src/headless/headless.h \
           src/headless/headlessImpl.hpp \
           src/gui/inspectors/chanInspector.h \
           src/gui/inspectors/filObjectInspector.h \
           src/gui/inspectors/inspectorProperties.h \
           src/gui/inspectors/segmentation/segInspector.h \
           src/gui/inspectors/segmentation/addSegmentButton.h \
           src/gui/inspectors/segObjectInspector.h \
           src/gui/inspectors/ui_chanInspector.h \
           src/gui/inspectors/volInspector.h \
           src/gui/inspectors/segmentation/exportButton.hpp \
           src/gui/inspectors/segmentation/exportButtonRaw.hpp \
           src/gui/inspectors/segmentation/exportMST.hpp \
           src/gui/inspectors/segmentation/exportSegmentList.hpp \
           src/gui/mainwindow.h \
           src/gui/menubar.h \
           src/gui/myInspectorWidget.h \
           src/gui/widgets/omSegmentContextMenu.h \
           src/gui/widgets/omCursors.h \
           src/gui/preferences/localPreferences2d.h \
           src/gui/preferences/localPreferences3d.h \
           src/gui/preferences/localPreferencesMeshing.h \
           src/gui/preferences/localPreferencesSystem.h \
           src/gui/preferences/preferences.h \
           src/gui/preferences/preferences2d.h \
           src/gui/preferences/preferences3d.h \
           src/gui/preferences/preferencesMesh.h \
           src/gui/preferences/ui_preferences3d.h \
           src/gui/recentFileList.h \
           src/gui/segmentLists/details/segmentListBase.h \
           src/gui/segmentLists/details/segmentListRecent.h \
           src/gui/segmentLists/details/segmentListValid.h \
           src/gui/segmentLists/details/segmentListWorking.h \
           src/gui/segmentLists/details/segmentListUncertain.h \
           src/gui/meshPreviewer/previewButton.hpp \
           src/gui/toolbars/dendToolbar/dendToolbar.h \
           src/gui/toolbars/dendToolbar/breakButton.h \
           src/gui/toolbars/dendToolbar/joinButton.h \
           src/gui/toolbars/dendToolbar/splitButton.h \
           src/gui/toolbars/dendToolbar/setValid.hpp \
           src/gui/toolbars/dendToolbar/setNotValid.hpp \
           src/gui/toolbars/dendToolbar/setUncertain.hpp \
           src/gui/toolbars/dendToolbar/setNotUncertain.hpp \
           src/gui/toolbars/dendToolbar/groupButtonTag.h \
           src/gui/toolbars/dendToolbar/showValidatedButton.h \
           src/gui/toolbars/dendToolbar/validationGroup.h \
           src/gui/toolbars/dendToolbar/graphTools.h \
           src/gui/toolbars/dendToolbar/displayTools.h \
           src/gui/toolbars/dendToolbar/breakThresholdGroup.h \
           src/gui/toolbars/dendToolbar/dust3DthresholdGroup.h \
           src/gui/toolbars/dendToolbar/thresholdGroup.h \
           src/gui/toolbars/mainToolbar/filterWidget.h \
           src/gui/toolbars/mainToolbar/mainToolbar.h \
           src/gui/toolbars/mainToolbar/navAndEditButtonGroup.h \
           src/gui/toolbars/mainToolbar/navAndEditButtons.h \
           src/gui/toolbars/mainToolbar/openViewGroupButton.h \
           src/gui/toolbars/mainToolbar/refreshVolumeButton.h \
           src/gui/toolbars/mainToolbar/saveButton.h \
           src/gui/toolbars/mainToolbar/toolButton.h \
           src/gui/toolbars/mainToolbar/refreshVolumeButton.h \
           src/gui/toolbars/mainToolbar/volumeComboBoxes.h \
           src/gui/toolbars/dendToolbar/autoBreakCheckbox.h \
           src/gui/toolbars/toolbarManager.h \
           src/gui/viewGroup.h \
           src/gui/viewGroupWidgetInfo.h \
           src/gui/widgets/omButton.h \
           src/gui/widgets/omCheckBox.h \
           src/gui/widgets/omSegmentListWidget.h \
           src/gui/widgets/omGroupListWidget.h \
           src/gui/widgets/omThresholdGroup.h \
           src/mesh/omMeshDrawer.h \
           src/mesh/omMeshTypes.h \
           src/mesh/omMipMesh.h \
           src/mesh/omMipMeshCoord.h \
           src/mesh/omMipMeshManager.h \
           src/mesh/omMeshSegmentList.h \
           src/mesh/omMeshSegmentListThread.h \
           src/mesh/ziMesher.h \
           src/mesh/ziMesherManager.h \
           src/mesh/ziMeshingChunk.h \
           src/project/omProject.h \
           src/actions/omProjectSaveAction.h \
           src/segment/actions/omSegmentEditor.h \
           src/actions/omSegmentJoinAction.h \
           src/actions/omSegmentUncertainAction.h \
           src/actions/omSegmentValidateAction.h \
           src/actions/omSegmentGroupAction.h \
           src/actions/omSegmentSelectAction.h \
           src/actions/omSegmentSplitAction.h \
           src/segment/details/omSegmentListContainer.hpp \
           src/segment/lowLevel/DynamicForestPool.hpp \
           src/segment/lowLevel/omPagingPtrStore.h \
           src/segment/lowLevel/omSegmentCacheImplLowLevel.h \
           src/segment/lowLevel/omSegmentGraph.h \
           src/segment/lowLevel/omSegmentIteratorLowLevel.h \
           src/segment/lowLevel/omSegmentListByMRU.h \
           src/segment/lowLevel/omSegmentListBySize.h \
           src/segment/io/omSegmentPage.hpp \
           src/segment/omSegment.h \
           src/segment/omSegmentCache.h \
           src/segment/omSegmentCacheImpl.h \
           src/segment/omSegmentColorizer.h \
           src/segment/omSegmentEdge.h \
           src/segment/omSegmentIterator.h \
           src/segment/omSegmentPointers.h \
           src/segment/omSegmentSelector.h \
           src/segment/omSegmentLists.hpp \
           src/segment/omSegmentValidation.hpp \
           src/segment/omSegmentUncertain.hpp \
           src/system/omGenericManager.h \
           src/system/events/omPreferenceEvent.h \
           src/system/events/omProgressEvent.h \
           src/system/events/omSegmentEvent.h \
           src/system/events/omToolModeEvent.h \
           src/system/events/omView3dEvent.h \
           src/system/events/omViewEvent.h \
           src/system/omAction.h \
           src/system/omBuildChannel.h \
           src/system/omBuildSegmentation.h \
           src/system/omBuildVolumes.h \
           src/system/cache/omLockedCacheObjects.hpp \
           src/system/cache/omCacheBase.h \
           src/system/cache/omCacheInfo.h \
           src/system/cache/omCacheManager.h \
           src/system/cache/omCacheGroup.h \
           src/system/events/omEvent.h \
           src/system/events/omEventManager.h \
           src/system/omGroup.h \
           src/system/omGroups.h \
           src/system/omLocalPreferences.h \
           src/system/omManageableObject.h \
           src/system/omPreferenceDefinitions.h \
           src/system/omPreferences.h \
           src/system/omProjectData.h \
           src/system/omStateManager.h \
           src/system/cache/omThreadedCache.h \
           src/system/cache/omHandleCacheMissThreaded.h \
           src/viewGroup/omViewGroupState.h \
           src/viewGroup/omBrushSize.hpp \
           src/viewGroup/omZoomLevel.hpp \
           src/utility/setUtilities.h \
           src/utility/omRand.hpp \
           src/utility/omSmartPtr.hpp \
           src/utility/omLockedObjects.h \
           src/utility/omLockedPODs.hpp \
           src/utility/dataWrappers.h \
           src/utility/dataWrapperContainer.hpp \
           src/utility/channelDataWrapper.hpp \
           src/utility/filterDataWrapper.hpp \
           src/utility/segmentDataWrapper.hpp \
           src/utility/segmentationDataWrapper.hpp \
           src/utility/fileHelpers.h \
           src/utility/localPrefFiles.h \
           src/utility/omImageDataIo.h \
           src/utility/omSystemInformation.h \
           src/utility/omTimer.h \
           src/utility/sortHelpers.h \
           src/utility/stringHelpers.h \
           src/utility/omThreadPool.hpp \
           src/utility/details/omIThreadPool.h \
           src/utility/details/omThreadPoolImpl.hpp \
           src/utility/details/omThreadPoolMock.hpp \
           src/utility/omChunkVoxelWalker.hpp \
           src/tiles/cache/omTileCache.h \
           src/tiles/cache/omTileCacheImpl.h \
           src/tiles/omTilePreFetcher.hpp \
           src/tiles/omTilePreFetcherTask.hpp \
           src/view2d/omPointsInCircle.hpp \
           src/view2d/details/omMidpointCircleAlgorithm.hpp \
           src/view2d/omLineDraw.hpp \
           src/view2d/omMouseEvents.hpp \
           src/view2d/omKeyEvents.hpp \
           src/view2d/omMouseEventWheel.hpp  \
           src/view2d/omMouseEventRelease.hpp \
           src/view2d/omMouseEventMove.hpp \
           src/view2d/omMouseEventPress.hpp \
           src/view2d/omMouseEventUtils.hpp \
           src/view2d/omView2dZoom.hpp \
           src/view2d/omScreenPainter.hpp \
           src/view2d/omTileDrawer.hpp \
           src/view2d/omView2dState.hpp \
           src/view2d/omView2dConverters.hpp \
           src/view2d/omDisplayInfo.hpp \
           src/view2d/omScreenShotSaver.hpp \
           src/view2d/omOnScreenTileCoords.h \
           src/tiles/omTextureID.h \
           src/tiles/omTile.h \
           src/tiles/omTileCoord.h \
           src/tiles/omTileDumper.hpp \
           src/tiles/omTileTypes.hpp \
           src/view2d/omView2d.h \
           src/view2d/omView2dCore.h \
           src/view2d/omView2dEvents.hpp \
           src/view3d/omCamera.h \
           src/view3d/omCameraMovement.h \
           src/view3d/omView3d.h \
           src/view3d/omView3dUi.h \
           src/view3d/omView3dWidget.h \
           src/view3d/widgets/omChunkExtentWidget.h \
           src/view3d/widgets/omInfoWidget.h \
           src/view3d/widgets/omSelectionWidget.h \
           src/view3d/widgets/omViewBoxWidget.h \
           src/view3d/widgets/omVolumeAxisWidget.h \
           src/volume/build/omDownsamplerTypes.hpp \
           src/volume/build/omDownsamplerVoxelTask.hpp \
           src/volume/build/omVolumeImporter.hpp \
           src/volume/build/omVolumeImporterHDF5.hpp \
           src/volume/build/omVolumeImporterImageStack.hpp \
           src/volume/build/omDownsampler.hpp \
           src/volume/omVolumeTypes.hpp \
           src/volume/omChunkData.hpp \
           src/volume/omChannel.h \
           src/actions/omVoxelSetValueAction.h \
           src/mesh/omDrawOptions.h \
           src/volume/omFilter2d.h \
           src/volume/omFilter2dManager.h \
           src/volume/omMipChunk.h \
           src/volume/omRawChunk.hpp \
           src/system/cache/omRawChunkCache.hpp \
           src/volume/omMipChunkCoord.h \
           src/volume/omMipVolume.h \
           src/volume/omSegmentation.h \
           src/system/cache/omMipVolumeCache.h \
           src/actions/omSegmentationThresholdChangeAction.h \
           src/system/cache/omMeshCache.h \
           src/volume/omVolume.h \
           src/mesh/omVolumeCuller.h \
           src/volume/build/omLoadImage.h \
           src/volume/omVolumeData.hpp \
           src/zi/base/base.h \
           src/zi/base/bash.h \
           src/zi/base/omni.h \
           src/zi/base/strings.h \
           src/zi/base/thrift.h \
           src/zi/base/time.h \
           src/zi/base/usages.h \
           src/zi/mesh/MarchingCubesTables.h \
           src/zi/mesh/ext/TriStrip/TriStrip_graph_array.h \
           src/zi/mesh/ext/TriStrip/TriStrip_heap_array.h \
           src/zi/mesh/ext/TriStrip/TriStripper.h \
           src/zi/zunit/zunit.h \
           src/zi/watershed/RawQuickieWS.h \
           tests/tests.hpp \
           tests/segment/segmentTests.hpp \
           tests/fakeMemMapFile.hpp \
           tests/testUtils.hpp

SOURCES += lib/strnatcmp.cpp \
           src/common/omCommon.cpp \
           src/common/omGl.cpp \
           src/segment/io/omMST.cpp \
           src/segment/io/omMSTold.cpp \
           src/datalayer/archive/omDataArchiveBoost.cpp \
           src/datalayer/archive/omDataArchiveCoords.cpp \
           src/datalayer/archive/omDataArchiveMipChunk.cpp \
           src/datalayer/archive/omDataArchiveProject.cpp \
           src/datalayer/archive/omDataArchiveSegment.cpp \
           src/datalayer/fs/omActionLoggerFS.cpp \
           src/datalayer/hdf5/omHdf5.cpp \
           src/datalayer/hdf5/omHdf5FileUtils.cpp \
           src/datalayer/hdf5/omHdf5Utils.cpp \
           src/datalayer/hdf5/omHdf5LowLevel.cpp \
           src/datalayer/hdf5/omHdf5Impl.cpp \
           src/datalayer/omDataLayer.cpp \
           src/datalayer/omDataPaths.cpp \
           src/gui/groupsTable.cpp \
           src/gui/meshPreviewer/meshPreviewer.cpp \
           src/gui/mstViewer.cpp \
           src/gui/cacheMonitorDialog.cpp \
           src/gui/cacheMonitorWidget.cpp \
           src/gui/guiUtils.cpp \
           src/headless/headless.cpp \
           src/gui/inspectors/chanInspector.cpp \
           src/gui/inspectors/filObjectInspector.cpp \
           src/gui/inspectors/inspectorProperties.cpp \
           src/gui/inspectors/segmentation/segInspector.cpp \
           src/gui/inspectors/segObjectInspector.cpp \
           src/gui/inspectors/volInspector.cpp \
           src/main.cpp \
           src/gui/mainwindow.cpp \
           src/gui/menubar.cpp \
           src/gui/myInspectorWidget.cpp \
           src/gui/widgets/omSegmentContextMenu.cpp \
           src/gui/widgets/omCursors.cpp \
           src/gui/widgets/omThresholdGroup.cpp \
           src/gui/preferences/localPreferences2d.cpp \
           src/gui/preferences/localPreferences3d.cpp \
           src/gui/preferences/localPreferencesMeshing.cpp \
           src/gui/preferences/localPreferencesSystem.cpp \
           src/gui/preferences/preferences.cpp \
           src/gui/preferences/preferences2d.cpp \
           src/gui/preferences/preferences3d.cpp \
           src/gui/preferences/preferencesMesh.cpp \
           src/gui/recentFileList.cpp \
           src/gui/segmentLists/elementListBox.cpp \
           src/gui/segmentLists/details/segmentListBase.cpp \
           src/gui/toolbars/dendToolbar/dendToolbar.cpp \
           src/gui/toolbars/dendToolbar/breakButton.cpp \
           src/gui/toolbars/dendToolbar/joinButton.cpp \
           src/gui/toolbars/dendToolbar/splitButton.cpp \
           src/gui/toolbars/dendToolbar/groupButtonTag.cpp \
           src/gui/toolbars/dendToolbar/showValidatedButton.cpp \
           src/gui/toolbars/dendToolbar/validationGroup.cpp \
           src/gui/toolbars/dendToolbar/graphTools.cpp \
           src/gui/toolbars/dendToolbar/displayTools.cpp \
           src/gui/toolbars/dendToolbar/breakThresholdGroup.cpp \
           src/gui/toolbars/dendToolbar/dust3DthresholdGroup.cpp \
           src/gui/toolbars/dendToolbar/thresholdGroup.cpp \
           src/gui/toolbars/dendToolbar/autoBreakCheckbox.cpp \
           src/gui/toolbars/mainToolbar/filterWidget.cpp \
           src/gui/toolbars/mainToolbar/mainToolbar.cpp \
           src/gui/toolbars/mainToolbar/navAndEditButtonGroup.cpp \
           src/gui/toolbars/mainToolbar/navAndEditButtons.cpp \
           src/gui/toolbars/mainToolbar/openViewGroupButton.cpp \
           src/gui/toolbars/mainToolbar/saveButton.cpp \
           src/gui/toolbars/mainToolbar/toolButton.cpp \
           src/gui/toolbars/mainToolbar/refreshVolumeButton.cpp \
           src/gui/toolbars/toolbarManager.cpp \
           src/gui/viewGroup.cpp \
           src/gui/widgets/omGroupListWidget.cpp \
           src/gui/widgets/omSegmentListWidget.cpp \
           src/mesh/omMeshDrawer.cpp \
           src/mesh/omMipMesh.cpp \
           src/mesh/omMipMeshCoord.cpp \
           src/mesh/omMipMeshManager.cpp \
           src/mesh/omMeshSegmentList.cpp \
           src/mesh/omMeshSegmentListThread.cpp \
           src/mesh/ziMesher.cpp \
           src/mesh/ziMesherManager.cpp \
           src/mesh/ziMeshingChunk.cpp \
           src/project/omProject.cpp \
           src/actions/omProjectSaveAction.cpp \
           src/actions/omSegmentJoinAction.cpp \
           src/actions/omSegmentUncertainAction.cpp \
           src/actions/omSegmentValidateAction.cpp \
           src/actions/omSegmentGroupAction.cpp \
           src/actions/omSegmentSelectAction.cpp \
           src/actions/omSegmentSplitAction.cpp \
           src/segment/lowLevel/omPagingPtrStore.cpp \
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
           src/segment/omSegmentSelector.cpp \
           src/system/events/omPreferenceEvent.cpp \
           src/system/events/omProgressEvent.cpp \
           src/system/events/omSegmentEvent.cpp \
           src/system/events/omToolModeEvent.cpp \
           src/system/events/omView3dEvent.cpp \
           src/system/events/omViewEvent.cpp \
           src/system/omAction.cpp \
           src/system/omBuildChannel.cpp \
           src/system/omBuildSegmentation.cpp \
           src/system/omBuildVolumes.cpp \
           src/system/cache/omCacheManager.cpp \
           src/system/cache/omCacheGroup.cpp \
           src/system/events/omEvent.cpp \
           src/system/events/omEventManager.cpp \
           src/system/omEvents.cpp \
           src/system/omGroups.cpp \
           src/system/omLocalPreferences.cpp \
           src/system/omPreferenceDefinitions.cpp \
           src/system/omProjectData.cpp \
           src/system/omStateManager.cpp \
           src/system/templatedClasses.cpp \
           src/tiles/cache/omTileCache.cpp \
           src/system/cache/omThreadedCache.cpp \
           src/viewGroup/omViewGroupState.cpp \
           src/utility/channelDataWrapper.cpp \
           src/utility/fileHelpers.cpp \
           src/utility/localPrefFiles.cpp \
           src/utility/omImageDataIo.cpp \
           src/utility/omSystemInformation.cpp \
           src/utility/stringHelpers.cpp \
           src/tiles/cache/omTileCacheImpl.cpp \
           src/tiles/omTilePreFetcher.cpp \
           src/tiles/omTilePreFetcherTask.cpp \
           src/view2d/omLineDraw.cpp \
           src/view2d/omTileDrawer.cpp \
           src/view2d/omOnScreenTileCoords.cpp \
           src/tiles/omTextureID.cpp \
           src/tiles/omTile.cpp \
           src/tiles/omTileCoord.cpp \
           src/tiles/omTileDumper.cpp \
           src/view2d/omView2d.cpp \
           src/view2d/omView2dCore.cpp \
           src/view3d/omCamera.cpp \
           src/view3d/omCameraMovement.cpp \
           src/view3d/omView3d.cpp \
           src/view3d/omView3dUi.cpp \
           src/view3d/widgets/omChunkExtentWidget.cpp \
           src/view3d/widgets/omInfoWidget.cpp \
           src/view3d/widgets/omSelectionWidget.cpp \
           src/view3d/widgets/omViewBoxWidget.cpp \
           src/volume/omVolumeTypes.cpp \
           src/volume/omChunkData.cpp \
           src/volume/omChannel.cpp \
           src/actions/omVoxelSetValueAction.cpp \
           src/volume/omFilter2d.cpp \
           src/volume/omFilter2dManager.cpp \
           src/volume/omMipChunk.cpp \
           src/volume/omMipChunkCoord.cpp \
           src/volume/omMipVolume.cpp \
           src/volume/omSegmentation.cpp \
           src/actions/omSegmentationThresholdChangeAction.cpp \
           src/system/cache/omMeshCache.cpp \
           src/volume/omVolume.cpp \
           src/mesh/omVolumeCuller.cpp \
           src/volume/omVolumeData.cpp \
           tests/utility/stringHelpersTest.cpp \
           src/zi/mesh/MarchingCubes.cpp \
           src/zi/mesh/QuadraticErrorSimplification.cpp \
           src/zi/mesh/ext/TriStrip/TriStripper.cpp \
           src/zi/watershed/RawQuickieWS.cpp

RESOURCES += src/gui/resources.qrc

INCLUDEPATH = src include lib tests
INCLUDEPATH += ../external/zi_lib
INCLUDEPATH += ../external/zi_lib/external/include

#### Windows
win32 {
   INCLUDEPATH += c:/hdf5lib/include C:/mygl
   LIBS += /omni/external/srcs/hdf5-1.8.4-patch1/src/.libs/libhdf5.a
   LIBS += -lgdi32
} else {
#### Linux or MacOS
   INCLUDEPATH +=  ../external/libs/HDF5/include
   LIBS += ../external/libs/HDF5/lib/libhdf5.a
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
