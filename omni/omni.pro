QT += opengl
TEMPLATE = app
CONFIG += qt

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
           src/actions/details \
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

## start of section to be rewritten using Perl
HEADERS +=  \
	lib/strnatcmp.h \
	src/actions/details/omAction.h \
	src/actions/details/omProjectCloseAction.h \
	src/actions/details/omProjectCloseActionImpl.hpp \
	src/actions/details/omProjectSaveAction.h \
	src/actions/details/omProjectSaveActionImpl.hpp \
	src/actions/details/omSegmentGroupAction.h \
	src/actions/details/omSegmentGroupActionImpl.hpp \
	src/actions/details/omSegmentJoinAction.h \
	src/actions/details/omSegmentJoinActionImpl.hpp \
	src/actions/details/omSegmentSelectAction.h \
	src/actions/details/omSegmentSelectActionImpl.hpp \
	src/actions/details/omSegmentSplitAction.h \
	src/actions/details/omSegmentSplitActionImpl.hpp \
	src/actions/details/omSegmentUncertainAction.h \
	src/actions/details/omSegmentUncertainActionImpl.hpp \
	src/actions/details/omSegmentValidateAction.h \
	src/actions/details/omSegmentValidateActionImpl.hpp \
	src/actions/details/omSegmentationThresholdChangeAction.h \
	src/actions/details/omSegmentationThresholdChangeActionImpl.hpp \
	src/actions/details/omVoxelSetValueAction.h \
	src/actions/details/omVoxelSetValueActionImpl.hpp \
	src/actions/io/omActionLogger.hpp \
	src/actions/io/omActionLoggerTask.hpp \
	src/actions/io/omActionOperators.h \
	src/actions/io/omActionReplayer.hpp \
	src/actions/omActions.hpp \
	src/common/om.hpp \
	src/common/omCommon.h \
	src/common/omDebug.h \
	src/common/omException.h \
	src/common/omGl.h \
	src/common/omStd.h \
	src/datalayer/archive/omDataArchiveBoost.h \
	src/datalayer/archive/omDataArchiveMipChunk.h \
	src/datalayer/archive/omDataArchiveProject.h \
	src/datalayer/archive/omDataArchiveSegment.h \
	src/datalayer/archive/omDataArchiveStd.hpp \
	src/datalayer/fs/omFileNames.hpp \
	src/datalayer/fs/omFileQT.hpp \
	src/datalayer/fs/omIOnDiskFile.h \
	src/datalayer/fs/omMemMappedFileQT.hpp \
	src/datalayer/fs/omMemMappedVolume.hpp \
	src/datalayer/fs/omVecInFile.hpp \
	src/datalayer/hdf5/omHdf5.h \
	src/datalayer/hdf5/omHdf5FileUtils.hpp \
	src/datalayer/hdf5/omHdf5Impl.h \
	src/datalayer/hdf5/omHdf5LowLevel.h \
	src/datalayer/hdf5/omHdf5Manager.h \
	src/datalayer/hdf5/omHdf5Utils.hpp \
	src/datalayer/omDataLayer.h \
	src/datalayer/omDataPath.h \
	src/datalayer/omDataPaths.h \
	src/datalayer/omDataWrapper.h \
	src/datalayer/omDummyWriter.h \
	src/datalayer/omIDataReader.h \
	src/datalayer/omIDataVolume.hpp \
	src/datalayer/omIDataWriter.h \
	src/datalayer/upgraders/omUpgradeTo14.hpp \
	src/datalayer/upgraders/omUpgradeTo20.hpp \
	src/datalayer/upgraders/omUpgraders.hpp \
	src/gui/cacheMonitorDialog.h \
	src/gui/cacheMonitorWidget.h \
	src/gui/groupsTable/dropdownMenuButton.hpp \
	src/gui/groupsTable/groupsTable.h \
	src/gui/guiUtils.hpp \
	src/gui/inspectors/chanInspector.h \
	src/gui/inspectors/filObjectInspector.h \
	src/gui/inspectors/inspectorProperties.h \
	src/gui/inspectors/segObjectInspector.h \
	src/gui/inspectors/segmentation/addSegmentButton.h \
	src/gui/inspectors/segmentation/buildButton.hpp \
	src/gui/inspectors/segmentation/exportButton.hpp \
	src/gui/inspectors/segmentation/exportButtonRaw.hpp \
	src/gui/inspectors/segmentation/exportMST.hpp \
	src/gui/inspectors/segmentation/exportSegmentList.hpp \
	src/gui/inspectors/segmentation/meshPreviewButton.hpp \
	src/gui/inspectors/segmentation/segInspector.h \
	src/gui/inspectors/ui_chanInspector.h \
	src/gui/inspectors/volInspector.h \
	src/gui/mainwindow.h \
	src/gui/menubar.h \
	src/gui/meshPreviewer/meshPreviewer.hpp \
	src/gui/meshPreviewer/previewButton.hpp \
	src/gui/meshPreviewer/scaleFactorLineEdit.hpp \
	src/gui/mstViewer.hpp \
	src/gui/myInspectorWidget.h \
	src/gui/omImageDialog.h \
	src/gui/preferences/checkboxHideCrosshairs.hpp \
	src/gui/preferences/localPreferences2d.h \
	src/gui/preferences/localPreferences3d.h \
	src/gui/preferences/localPreferencesMeshing.h \
	src/gui/preferences/localPreferencesSystem.h \
	src/gui/preferences/preferences.h \
	src/gui/preferences/preferences2d.h \
	src/gui/preferences/preferences3d.h \
	src/gui/preferences/preferencesMesh.h \
	src/gui/preferences/spinBoxCrosshairOpeningSize.hpp \
	src/gui/preferences/ui_preferences3d.h \
	src/gui/recentFileList.h \
	src/gui/segmentLists/details/segmentListBase.h \
	src/gui/segmentLists/details/segmentListRecent.h \
	src/gui/segmentLists/details/segmentListUncertain.h \
	src/gui/segmentLists/details/segmentListValid.h \
	src/gui/segmentLists/details/segmentListWorking.h \
	src/gui/segmentLists/elementListBox.hpp \
	src/gui/segmentLists/elementListBoxImpl.hpp \
	src/gui/segmentLists/segmentListKeyPressEventListener.h \
	src/gui/toolbars/dendToolbar/alphaVegasButton.hpp \
	src/gui/toolbars/dendToolbar/breakButton.h \
	src/gui/toolbars/dendToolbar/breakThresholdGroup.h \
	src/gui/toolbars/dendToolbar/cutButton.h \
	src/gui/toolbars/dendToolbar/dendToolbar.h \
	src/gui/toolbars/dendToolbar/displayTools/brightnessSpinBox.hpp \
	src/gui/toolbars/dendToolbar/displayTools/contrastSpinBox.hpp \
	src/gui/toolbars/dendToolbar/displayTools/displayTools.h \
	src/gui/toolbars/dendToolbar/displayTools/dust3DthresholdGroup.hpp \
	src/gui/toolbars/dendToolbar/displayTools/gammaSpinBox.hpp \
	src/gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxBase.hpp \
	src/gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxX.hpp \
	src/gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxY.hpp \
	src/gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxZ.hpp \
	src/gui/toolbars/dendToolbar/graphTools.h \
	src/gui/toolbars/dendToolbar/groupButtonTag.h \
	src/gui/toolbars/dendToolbar/joinButton.h \
	src/gui/toolbars/dendToolbar/setNotUncertain.hpp \
	src/gui/toolbars/dendToolbar/setNotValid.hpp \
	src/gui/toolbars/dendToolbar/setUncertain.hpp \
	src/gui/toolbars/dendToolbar/setValid.hpp \
	src/gui/toolbars/dendToolbar/showValidatedButton.h \
	src/gui/toolbars/dendToolbar/splitButton.h \
	src/gui/toolbars/dendToolbar/thresholdGroup.h \
	src/gui/toolbars/dendToolbar/validationGroup.h \
	src/gui/toolbars/mainToolbar/filterToBlackCheckbox.hpp \
	src/gui/toolbars/mainToolbar/filterWidget.hpp \
	src/gui/toolbars/mainToolbar/filterWidgetImpl.hpp \
	src/gui/toolbars/mainToolbar/mainToolbar.h \
	src/gui/toolbars/mainToolbar/navAndEditButtonGroup.h \
	src/gui/toolbars/mainToolbar/navAndEditButtons.h \
	src/gui/toolbars/mainToolbar/openViewGroupButton.h \
	src/gui/toolbars/mainToolbar/refreshVolumeButton.h \
	src/gui/toolbars/mainToolbar/saveButton.h \
	src/gui/toolbars/mainToolbar/toolButton.h \
	src/gui/toolbars/mainToolbar/volumeComboBoxes.h \
	src/gui/toolbars/toolbarManager.h \
	src/gui/updateSegmentProperties.hpp \
	src/gui/viewGroup.h \
	src/gui/viewGroupWidgetInfo.h \
	src/gui/widgets/omButton.hpp \
	src/gui/widgets/omCheckBoxWidget.hpp \
	src/gui/widgets/omCursors.h \
	src/gui/widgets/omDoubleSpinBox.hpp \
	src/gui/widgets/omGroupListWidget.h \
	src/gui/widgets/omIntSpinBox.hpp \
	src/gui/widgets/omLabelHBox.hpp \
	src/gui/widgets/omLineEdit.hpp \
	src/gui/widgets/omProgressBar.hpp \
	src/gui/widgets/omSegmentContextMenu.h \
	src/gui/widgets/omSegmentListWidget.h \
	src/gui/widgets/omWidget.hpp \
	src/headless/headless.h \
	src/headless/headlessImpl.hpp \
	src/mainpage.h \
	src/mesh/detail/MipChunkMeshCollector.hpp \
	src/mesh/detail/TriStripCollector.hpp \
	src/mesh/drawer/omFindChunksToDraw.hpp \
	src/mesh/drawer/omMeshDrawPlanner.hpp \
	src/mesh/drawer/omMeshDrawer.h \
	src/mesh/drawer/omMeshDrawerImpl.hpp \
	src/mesh/drawer/omMeshPlan.h \
	src/mesh/drawer/omMeshPlanCache.hpp \
	src/mesh/drawer/omMeshSegmentList.hpp \
	src/mesh/drawer/omMeshSegmentListTask.h \
	src/mesh/drawer/omMeshSegmentListTypes.hpp \
	src/mesh/io/omDataForMeshLoad.hpp \
	src/mesh/io/omMeshConvertV1toV2.hpp \
	src/mesh/io/omMeshConvertV1toV2Task.hpp \
	src/mesh/io/omMeshMetadata.hpp \
	src/mesh/io/v1/omMeshReaderV1.hpp \
	src/mesh/io/v2/chunk/omMemMappedAllocFile.hpp \
	src/mesh/io/v2/chunk/omMeshChunkAllocTable.hpp \
	src/mesh/io/v2/chunk/omMeshChunkDataReaderV2.hpp \
	src/mesh/io/v2/chunk/omMeshChunkDataWriterTaskV2.hpp \
	src/mesh/io/v2/chunk/omMeshChunkDataWriterV2.hpp \
	src/mesh/io/v2/chunk/omMeshChunkTypes.h \
	src/mesh/io/v2/omMeshFilePtrCache.hpp \
	src/mesh/io/v2/omMeshReaderV2.hpp \
	src/mesh/io/v2/omMeshWriterV2.hpp \
	src/mesh/io/v2/omRingBuffer.hpp \
	src/mesh/io/v2/threads/omMeshWriterTaskV2.hpp \
	src/mesh/omDrawOptions.h \
	src/mesh/omMeshParams.hpp \
	src/mesh/omMeshTypes.h \
	src/mesh/omMipMesh.h \
	src/mesh/omMipMeshCoord.h \
	src/mesh/omMipMeshManager.h \
	src/mesh/omVolumeCuller.h \
	src/mesh/ziMesher.hpp \
	src/project/omProject.h \
	src/project/omProjectDataImpl.hpp \
	src/segment/details/omSegmentListBySize2.hpp \
	src/segment/details/omSegmentListContainer.hpp \
	src/segment/details/omSegmentListsTypes.hpp \
	src/segment/details/sortedRootSegments.hpp \
	src/segment/details/sortedRootSegmentsTypes.h \
	src/segment/details/sortedRootSegmentsVector.hpp \
	src/segment/io/omMST.h \
	src/segment/io/omMSTold.h \
	src/segment/io/omSegmentPage.hpp \
	src/segment/io/omUserEdges.hpp \
	src/segment/io/omValidGroupNum.hpp \
	src/segment/lowLevel/DynamicForestPool.hpp \
	src/segment/lowLevel/omPagingPtrStore.h \
	src/segment/lowLevel/omSegmentCacheImplLowLevel.h \
	src/segment/lowLevel/omSegmentGraph.h \
	src/segment/lowLevel/omSegmentIteratorLowLevel.h \
	src/segment/lowLevel/omSegmentListByMRU.h \
	src/segment/lowLevel/omSegmentListBySize.h \
	src/segment/omFindCommonEdge.hpp \
	src/segment/omSegment.h \
	src/segment/omSegmentCache.h \
	src/segment/omSegmentCacheImpl.h \
	src/segment/omSegmentColorizer.h \
	src/segment/omSegmentEdge.h \
	src/segment/omSegmentIterator.h \
	src/segment/omSegmentLists.hpp \
	src/segment/omSegmentPointers.h \
	src/segment/omSegmentSearched.hpp \
	src/segment/omSegmentSelected.hpp \
	src/segment/omSegmentSelector.h \
	src/segment/omSegmentUncertain.hpp \
	src/segment/omSegmentUtils.hpp \
	src/segment/omSegmentValidation.hpp \
	src/system/cache/omCacheBase.h \
	src/system/cache/omCacheGroup.h \
	src/system/cache/omCacheInfo.h \
	src/system/cache/omCacheManager.h \
	src/system/cache/omCacheManagerImpl.hpp \
	src/system/cache/omHandleCacheMissTask.hpp \
	src/system/cache/omLockedCacheObjects.hpp \
	src/system/cache/omMeshCache.h \
	src/system/cache/omMipVolumeCache.h \
	src/system/cache/omRawChunkCache.hpp \
	src/system/cache/omThreadedCache.h \
	src/system/events/omEvent.h \
	src/system/events/omEventManager.h \
	src/system/events/omKeyPressEventListener.h \
	src/system/events/omPreferenceEvent.h \
	src/system/events/omProgressEvent.h \
	src/system/events/omSegmentEvent.h \
	src/system/events/omToolModeEvent.h \
	src/system/events/omView3dEvent.h \
	src/system/events/omViewEvent.h \
	src/system/omAlphaVegasMode.hpp \
	src/system/omBuildVolumes.h \
	src/system/omEvents.h \
	src/system/omGarbage.h \
	src/system/omGenericManager.h \
	src/system/omGroup.h \
	src/system/omGroups.h \
	src/system/omLocalPreferences.hpp \
	src/system/omManageableObject.h \
	src/system/omPreferenceDefinitions.h \
	src/system/omPreferences.h \
	src/system/omProjectData.h \
	src/system/omStateManager.h \
	src/tiles/cache/omTileCache.h \
	src/tiles/cache/omTileCacheChannel.hpp \
	src/tiles/cache/omTileCacheImpl.hpp \
	src/tiles/cache/omTileCacheSegmentation.hpp \
	src/tiles/omTextureID.h \
	src/tiles/omTile.h \
	src/tiles/omTileCoord.h \
	src/tiles/omTileDumper.hpp \
	src/tiles/omTilePreFetcher.h \
	src/tiles/omTilePreFetcherTask.hpp \
	src/tiles/omTileTypes.hpp \
	src/utility/channelDataWrapper.hpp \
	src/utility/dataWrapperContainer.hpp \
	src/utility/dataWrappers.h \
	src/utility/details/omIThreadPool.h \
	src/utility/details/omThreadPoolImpl.hpp \
	src/utility/details/omThreadPoolMock.hpp \
	src/utility/fileHelpers.h \
	src/utility/filterDataWrapper.hpp \
	src/utility/image/bits/omImage_traits.hpp \
	src/utility/image/omFilterImage.hpp \
	src/utility/image/omImage.hpp \
	src/utility/localPrefFiles.h \
	src/utility/localPrefFilesImpl.hpp \
	src/utility/omChunkVoxelWalker.hpp \
	src/utility/omLockedObjects.h \
	src/utility/omLockedPODs.hpp \
	src/utility/omRand.hpp \
	src/utility/omSmartPtr.hpp \
	src/utility/omSystemInformation.h \
	src/utility/omThreadPool.hpp \
	src/utility/omThreadPoolManager.h \
	src/utility/omTimer.h \
	src/utility/segmentDataWrapper.hpp \
	src/utility/segmentationDataWrapper.hpp \
	src/utility/setUtilities.h \
	src/utility/sortHelpers.h \
	src/utility/stringHelpers.h \
	src/view2d/details/omMidpointCircleAlgorithm.hpp \
	src/view2d/om2dPreferences.hpp \
	src/view2d/omDisplayInfo.hpp \
	src/view2d/omKeyEvents.hpp \
	src/view2d/omLineDraw.hpp \
	src/view2d/omMouseEventMove.hpp \
	src/view2d/omMouseEventPress.hpp \
	src/view2d/omMouseEventRelease.hpp \
	src/view2d/omMouseEventUtils.hpp \
	src/view2d/omMouseEventWheel.hpp \
	src/view2d/omMouseEvents.hpp \
	src/view2d/omOnScreenTileCoords.h \
	src/view2d/omPointsInCircle.hpp \
	src/view2d/omScreenPainter.hpp \
	src/view2d/omScreenShotSaver.hpp \
	src/view2d/omTileDrawer.h \
	src/view2d/omView2d.h \
	src/view2d/omView2dConverters.hpp \
	src/view2d/omView2dCore.h \
	src/view2d/omView2dEvents.hpp \
	src/view2d/omView2dKeyPressEventListener.h \
	src/view2d/omView2dState.hpp \
	src/view2d/omView2dZoom.hpp \
	src/view3d/om3dPreferences.hpp \
	src/view3d/omCamera.h \
	src/view3d/omCameraMovement.h \
	src/view3d/omView3d.h \
	src/view3d/omView3dKeyPressEventListener.h \
	src/view3d/omView3dUi.h \
	src/view3d/omView3dWidget.h \
	src/view3d/widgets/omChunkExtentWidget.h \
	src/view3d/widgets/omInfoWidget.h \
	src/view3d/widgets/omPercDone.hpp \
	src/view3d/widgets/omSelectionWidget.h \
	src/view3d/widgets/omViewBoxWidget.h \
	src/view3d/widgets/omVolumeAxisWidget.h \
	src/viewGroup/omBrushSize.hpp \
	src/viewGroup/omViewGroupState.h \
	src/viewGroup/omZoomLevel.hpp \
	src/volume/build/omBuildChannel.hpp \
	src/volume/build/omBuildSegmentation.hpp \
	src/volume/build/omDownsampler.hpp \
	src/volume/build/omDownsamplerTypes.hpp \
	src/volume/build/omDownsamplerVoxelTask.hpp \
	src/volume/build/omLoadImage.h \
	src/volume/build/omProcessSegmentationChunk.hpp \
	src/volume/build/omVolumeAllocater.hpp \
	src/volume/build/omVolumeBuilder.hpp \
	src/volume/build/omVolumeImporter.hpp \
	src/volume/build/omVolumeImporterHDF5.hpp \
	src/volume/build/omVolumeImporterImageStack.hpp \
	src/volume/omChannel.h \
	src/volume/omChunkData.hpp \
	src/volume/omFilter2d.h \
	src/volume/omFilter2dManager.h \
	src/volume/omMipChunk.h \
	src/volume/omMipChunkCoord.h \
	src/volume/omMipChunkData.hpp \
	src/volume/omMipVolume.h \
	src/volume/omRawChunk.hpp \
	src/volume/omSegmentation.h \
	src/volume/omVolume.h \
	src/volume/omVolumeData.hpp \
	src/volume/omVolumeTypes.hpp \
	src/zi/base/base.h \
	src/zi/base/bash.h \
	src/zi/base/bits/system.h \
	src/zi/base/strings.h \
	src/zi/base/system.h \
	src/zi/base/thrift.h \
	src/zi/base/time.h \
	src/zi/base/usages.h \
	src/zi/matlab/zmex.hpp \
	src/zi/omMutex.h \
	src/zi/omThreads.h \
	src/zi/omUtility.h \
	src/zi/trees/DisjointSets.hpp \
	src/zi/trees/lib/DynaTree.hpp \
	src/zi/trees/lib/DynamicTree.hpp \
	src/zi/trees/lib/FHeap.hpp \
	src/zi/watershed/MemMap.hpp \
	src/zi/watershed/RawQuickieWS.h \
	tests/cache/lockedObjectsTests.hpp \
	tests/datalayer/omMeshChunkAllocTableTests.hpp \
	tests/datalayer/vecInFileTests.hpp \
	tests/fakeMemMapFile.hpp \
	tests/segment/mockSegments.hpp \
	tests/segment/omSegmentListBySizeTests.hpp \
	tests/segment/segmentTests.hpp \
	tests/segment/segmentTests1.hpp \
	tests/testUtils.hpp \
	tests/tests.hpp \
	tests/utility/stringHelpersTest.h

SOURCES +=  \
	lib/strnatcmp.cpp \
	src/actions/details/omProjectCloseAction.cpp \
	src/actions/details/omProjectSaveAction.cpp \
	src/actions/details/omSegmentGroupAction.cpp \
	src/actions/details/omSegmentJoinAction.cpp \
	src/actions/details/omSegmentSelectAction.cpp \
	src/actions/details/omSegmentSplitAction.cpp \
	src/actions/details/omSegmentUncertainAction.cpp \
	src/actions/details/omSegmentValidateAction.cpp \
	src/actions/details/omSegmentationThresholdChangeAction.cpp \
	src/actions/details/omVoxelSetValueAction.cpp \
	src/actions/io/omActionOperators.cpp \
	src/actions/io/omActionReplayer.cpp \
	src/actions/omActions.cpp \
	src/common/omCommon.cpp \
	src/common/omGl.cpp \
	src/datalayer/archive/omDataArchiveBoost.cpp \
	src/datalayer/archive/omDataArchiveMipChunk.cpp \
	src/datalayer/archive/omDataArchiveProject.cpp \
	src/datalayer/archive/omDataArchiveSegment.cpp \
	src/datalayer/hdf5/omHdf5.cpp \
	src/datalayer/hdf5/omHdf5FileUtils.cpp \
	src/datalayer/hdf5/omHdf5Impl.cpp \
	src/datalayer/hdf5/omHdf5LowLevel.cpp \
	src/datalayer/hdf5/omHdf5Utils.cpp \
	src/datalayer/omDataLayer.cpp \
	src/datalayer/omDataPaths.cpp \
	src/gui/cacheMonitorDialog.cpp \
	src/gui/cacheMonitorWidget.cpp \
	src/gui/groupsTable/groupsTable.cpp \
	src/gui/inspectors/chanInspector.cpp \
	src/gui/inspectors/filObjectInspector.cpp \
	src/gui/inspectors/inspectorProperties.cpp \
	src/gui/inspectors/segObjectInspector.cpp \
	src/gui/inspectors/segmentation/segInspector.cpp \
	src/gui/mainwindow.cpp \
	src/gui/menubar.cpp \
	src/gui/meshPreviewer/meshPreviewer.cpp \
	src/gui/mstViewer.cpp \
	src/gui/myInspectorWidget.cpp \
	src/gui/preferences/localPreferences2d.cpp \
	src/gui/preferences/localPreferences3d.cpp \
	src/gui/preferences/localPreferencesMeshing.cpp \
	src/gui/preferences/localPreferencesSystem.cpp \
	src/gui/preferences/preferences.cpp \
	src/gui/preferences/preferences2d.cpp \
	src/gui/preferences/preferences3d.cpp \
	src/gui/preferences/preferencesMesh.cpp \
	src/gui/recentFileList.cpp \
	src/gui/segmentLists/details/segmentListBase.cpp \
	src/gui/segmentLists/elementListBox.cpp \
	src/gui/toolbars/dendToolbar/breakButton.cpp \
	src/gui/toolbars/dendToolbar/cutButton.cpp \
	src/gui/toolbars/dendToolbar/dendToolbar.cpp \
	src/gui/toolbars/dendToolbar/displayTools/displayTools.cpp \
	src/gui/toolbars/dendToolbar/graphTools.cpp \
	src/gui/toolbars/dendToolbar/groupButtonTag.cpp \
	src/gui/toolbars/dendToolbar/joinButton.cpp \
	src/gui/toolbars/dendToolbar/showValidatedButton.cpp \
	src/gui/toolbars/dendToolbar/splitButton.cpp \
	src/gui/toolbars/dendToolbar/validationGroup.cpp \
	src/gui/toolbars/mainToolbar/mainToolbar.cpp \
	src/gui/toolbars/mainToolbar/navAndEditButtonGroup.cpp \
	src/gui/toolbars/mainToolbar/navAndEditButtons.cpp \
	src/gui/toolbars/mainToolbar/openViewGroupButton.cpp \
	src/gui/toolbars/mainToolbar/refreshVolumeButton.cpp \
	src/gui/toolbars/mainToolbar/saveButton.cpp \
	src/gui/toolbars/mainToolbar/toolButton.cpp \
	src/gui/toolbars/toolbarManager.cpp \
	src/gui/viewGroup.cpp \
	src/gui/widgets/omCursors.cpp \
	src/gui/widgets/omGroupListWidget.cpp \
	src/gui/widgets/omSegmentContextMenu.cpp \
	src/gui/widgets/omSegmentListWidget.cpp \
	src/headless/headless.cpp \
	src/main.cpp \
	src/mesh/drawer/omMeshDrawer.cpp \
	src/mesh/drawer/omMeshSegmentListTask.cpp \
	src/mesh/io/v2/chunk/omMeshChunkAllocTable.cpp \
	src/mesh/omMipMesh.cpp \
	src/mesh/omMipMeshCoord.cpp \
	src/mesh/omMipMeshManager.cpp \
	src/mesh/omVolumeCuller.cpp \
	src/project/omProject.cpp \
	src/segment/io/omMST.cpp \
	src/segment/io/omMSTold.cpp \
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
	src/segment/omSegmentIterator.cpp \
	src/segment/omSegmentSelector.cpp \
	src/system/cache/omCacheGroup.cpp \
	src/system/cache/omCacheManager.cpp \
	src/system/cache/omMeshCache.cpp \
	src/system/cache/omThreadedCache.cpp \
	src/system/events/omEvent.cpp \
	src/system/events/omEventManager.cpp \
	src/system/events/omPreferenceEvent.cpp \
	src/system/events/omProgressEvent.cpp \
	src/system/events/omSegmentEvent.cpp \
	src/system/events/omToolModeEvent.cpp \
	src/system/events/omView3dEvent.cpp \
	src/system/events/omViewEvent.cpp \
	src/system/omEvents.cpp \
	src/system/omGroups.cpp \
	src/system/omPreferenceDefinitions.cpp \
	src/system/omProjectData.cpp \
	src/system/omStateManager.cpp \
	src/system/templatedClasses.cpp \
	src/tiles/cache/omTileCache.cpp \
	src/tiles/omTextureID.cpp \
	src/tiles/omTile.cpp \
	src/tiles/omTileCoord.cpp \
	src/tiles/omTileDumper.cpp \
	src/tiles/omTilePreFetcher.cpp \
	src/tiles/omTilePreFetcherTask.cpp \
	src/utility/channelDataWrapper.cpp \
	src/utility/fileHelpers.cpp \
	src/utility/omSystemInformation.cpp \
	src/utility/omThreadPoolManager.cpp \
	src/utility/stringHelpers.cpp \
	src/view2d/omLineDraw.cpp \
	src/view2d/omOnScreenTileCoords.cpp \
	src/view2d/omTileDrawer.cpp \
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
	src/viewGroup/omViewGroupState.cpp \
	src/volume/omChannel.cpp \
	src/volume/omChunkData.cpp \
	src/volume/omFilter2d.cpp \
	src/volume/omFilter2dManager.cpp \
	src/volume/omMipChunk.cpp \
	src/volume/omMipChunkCoord.cpp \
	src/volume/omMipVolume.cpp \
	src/volume/omSegmentation.cpp \
	src/volume/omVolume.cpp \
	src/volume/omVolumeData.cpp \
	src/volume/omVolumeTypes.cpp \
	src/zi/watershed/RawQuickieWS.cpp \
	tests/segment/mockSegments.cpp \
	tests/utility/stringHelpersTest.cpp
## end of section to be rewritten using Perl

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

####
# if gcc > 4.1, make non-void functions not returning something generate an error
# from http://stackoverflow.com/questions/801279/finding-compiler-vendor-version-using-qmake
linux-g++ {
    system( g++ --version | grep -e "4.[2-9]" ) {
        message( g++ version 4.[2-9] found )
        CONFIG += g++4new
    }
    else {
        CONFIG += g++old
    }
}
# http://gcc.gnu.org/bugzilla/show_bug.cgi?id=43943
g++4new: QMAKE_CXXFLAGS += -Werror=return-type

####
# debug/release
# if mac or ./omni/release files exists, force release mode; else debug
CONFIG -= debug
CONFIG -= release
mac {
    CONFIG += release
} else {
    exists(release) {
       CONFIG += release
    }else {
       CONFIG += debug
    }
}
CONFIG(release, debug|release) {
    message ( in release mode; adding NDEBUG and no-strict-aliasing )
    QMAKE_CXXFLAGS += -DNDEBUG  -fno-strict-aliasing
    QMAKE_LFLAGS   += -DNDEBUG
}


# GCC Parallel Mode support
linux-g++ {
    system( g++ --version | grep -e "4.[3-9]" ) {
        message( assuming g++ Parallel Mode supported )
        QMAKE_CXXFLAGS += -DZI_USE_OPENMP -fopenmp
        QMAKE_LFLAGS   += -DZI_USE_OPENMP -fopenmp
    }
}

