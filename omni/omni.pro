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
	src/actions/details/omActionBase.hpp \
	src/actions/details/omActionImpls.hpp \
	src/actions/details/omProjectCloseActionImpl.hpp \
	src/actions/details/omProjectSaveActionImpl.hpp \
	src/actions/details/omSegmentGroupActionImpl.hpp \
	src/actions/details/omSegmentJoinActionImpl.hpp \
	src/actions/details/omSegmentSelectActionImpl.hpp \
	src/actions/details/omSegmentSplitAction.h \
	src/actions/details/omSegmentSplitActionImpl.hpp \
	src/actions/details/omSegmentUncertainAction.h \
	src/actions/details/omSegmentUncertainActionImpl.hpp \
	src/actions/details/omSegmentValidateAction.h \
	src/actions/details/omSegmentValidateActionImpl.hpp \
	src/actions/details/omSegmentationThresholdChangeActionImpl.hpp \
	src/actions/details/omUndoCommand.hpp \
	src/actions/details/omVoxelSetValueAction.h \
	src/actions/details/omVoxelSetValueActionImpl.hpp \
	src/actions/io/omActionLogger.hpp \
	src/actions/io/omActionLoggerTask.hpp \
	src/actions/io/omActionOperators.h \
	src/actions/io/omActionReplayer.hpp \
	src/actions/omActions.h \
	src/actions/omSelectSegmentParams.hpp \
	src/chunks/details/omPtrToChunkDataBase.hpp \
	src/chunks/details/omPtrToChunkDataMemMapVol.h \
	src/chunks/omChunk.h \
	src/chunks/omChunkCache.hpp \
	src/chunks/omChunkCoord.h \
	src/chunks/omChunkData.h \
	src/chunks/omChunkMipping.hpp \
	src/chunks/omChunkTypes.hpp \
	src/chunks/omChunkUtils.hpp \
	src/chunks/omRawChunk.hpp \
	src/chunks/omRawChunkMemMapped.hpp \
	src/chunks/omSegChunk.h \
	src/chunks/omSegChunkData.h \
	src/chunks/omSegChunkDataVisitors.hpp \
	src/chunks/uniqueValues/omChunkUniqueValuesManager.hpp \
	src/chunks/uniqueValues/omChunkUniqueValuesPerThreshold.hpp \
	src/chunks/uniqueValues/omChunkUniqueValuesTypes.h \
	src/chunks/uniqueValues/omThresholdsInChunk.hpp \
	src/common/om.hpp \
	src/common/omBoost.h \
	src/common/omCommon.h \
	src/common/omDebug.h \
	src/common/omException.h \
	src/common/omGl.h \
	src/common/omSet.hpp \
	src/common/omStd.h \
	src/common/omString.hpp \
	src/datalayer/archive/omDataArchiveBoost.h \
	src/datalayer/archive/omDataArchiveProject.h \
	src/datalayer/archive/omDataArchiveSegment.h \
	src/datalayer/archive/omDataArchiveStd.hpp \
	src/datalayer/archive/omDataArchiveWrappers.h \
	src/datalayer/archive/omMetadataSegmentation.h \
	src/datalayer/archive/omMipVolumeArchive.h \
	src/datalayer/archive/omMipVolumeArchiveOld.h \
	src/datalayer/fs/omFileNames.hpp \
	src/datalayer/fs/omFileQT.hpp \
	src/datalayer/fs/omIOnDiskFile.h \
	src/datalayer/fs/omMemMappedFileQT.hpp \
	src/datalayer/fs/omVecInFile.hpp \
	src/datalayer/hdf5/omExportVolToHdf5.hpp \
	src/datalayer/hdf5/omHdf5.h \
	src/datalayer/hdf5/omHdf5ChunkUtils.hpp \
	src/datalayer/hdf5/omHdf5FileUtils.hpp \
	src/datalayer/hdf5/omHdf5Impl.h \
	src/datalayer/hdf5/omHdf5LowLevel.h \
	src/datalayer/hdf5/omHdf5Manager.h \
	src/datalayer/hdf5/omHdf5Utils.hpp \
	src/datalayer/omDataPath.h \
	src/datalayer/omDataPaths.h \
	src/datalayer/omDataWrapper.h \
	src/datalayer/omDummyWriter.h \
	src/datalayer/omIDataVolume.hpp \
	src/datalayer/upgraders/omUpgradeTo14.hpp \
	src/datalayer/upgraders/omUpgradeTo20.hpp \
	src/datalayer/upgraders/omUpgraders.hpp \
	src/events/omEvent.h \
	src/events/omEventManager.h \
	src/events/omKeyPressEventListener.h \
	src/events/omPreferenceEvent.h \
	src/events/omProgressEvent.h \
	src/events/omSegmentEvent.h \
	src/events/omToolModeEvent.h \
	src/events/omView3dEvent.h \
	src/events/omViewEvent.h \
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
	src/gui/widgets/omNewFileDialog.hpp \
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
	src/mesh/omMipMeshManagers.hpp \
	src/mesh/omVolumeCuller.h \
	src/mesh/ziMesher.hpp \
	src/project/omAffinityGraphManager.h \
	src/project/omChannelManager.h \
	src/project/omProject.h \
	src/project/omProjectGlobals.h \
	src/project/omProjectImpl.hpp \
	src/project/omProjectVolumes.h \
	src/project/omSegmentationManager.h \
	src/segment/details/omSegmentListBySize2.hpp \
	src/segment/details/omSegmentListContainer.hpp \
	src/segment/details/omSegmentListsTypes.hpp \
	src/segment/details/sortedRootSegments.hpp \
	src/segment/details/sortedRootSegmentsTypes.h \
	src/segment/details/sortedRootSegmentsVector.hpp \
	src/segment/io/omMST.h \
	src/segment/io/omMSTold.h \
	src/segment/io/omMSTtypes.h \
	src/segment/io/omSegmentPage.hpp \
	src/segment/io/omUserEdges.hpp \
	src/segment/io/omValidGroupNum.hpp \
	src/segment/lowLevel/DynamicForestPool.hpp \
	src/segment/lowLevel/omDynamicForestCache.hpp \
	src/segment/lowLevel/omEnabledSegments.hpp \
	src/segment/lowLevel/omPagingPtrStore.h \
	src/segment/lowLevel/omSegmentBags.hpp \
	src/segment/lowLevel/omSegmentCacheImplLowLevel.h \
	src/segment/lowLevel/omSegmentGraph.h \
	src/segment/lowLevel/omSegmentIteratorLowLevel.h \
	src/segment/lowLevel/omSegmentListByMRU.h \
	src/segment/lowLevel/omSegmentListBySize.h \
	src/segment/lowLevel/omSegmentSelection.hpp \
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
	src/system/cache/omGetSetCache.hpp \
	src/system/cache/omHandleCacheMissTask.hpp \
	src/system/cache/omLockedCacheObjects.hpp \
	src/system/cache/omMeshCache.h \
	src/system/cache/omThreadedCache.h \
	src/system/cache/omVolSliceCache.hpp \
	src/system/omAlphaVegasMode.hpp \
	src/system/omAppState.hpp \
	src/system/omEvents.h \
	src/system/omGarbage.h \
	src/system/omGenericManager.h \
	src/system/omGroup.h \
	src/system/omGroups.h \
	src/system/omLocalPreferences.hpp \
	src/system/omManageableObject.h \
	src/system/omPreferenceDefinitions.h \
	src/system/omPreferences.h \
	src/system/omQTApp.hpp \
	src/system/omStateManager.h \
	src/system/omStateManagerImpl.hpp \
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
	src/utility/filterDataWrapper.hpp \
	src/utility/fuzzyStdObjs.hpp \
	src/utility/image/bits/omImage_traits.hpp \
	src/utility/image/omFilterImage.hpp \
	src/utility/image/omImage.hpp \
	src/utility/localPrefFiles.h \
	src/utility/localPrefFilesImpl.hpp \
	src/utility/omChunkVoxelWalker.hpp \
	src/utility/omColorUtils.hpp \
	src/utility/omCopyFirstN.hpp \
	src/utility/omFileHelpers.h \
	src/utility/omLockedObjects.h \
	src/utility/omLockedPODs.hpp \
	src/utility/omRand.hpp \
	src/utility/omRandColorFile.hpp \
	src/utility/omSimpleProgress.hpp \
	src/utility/omSmartPtr.hpp \
	src/utility/omStringHelpers.h \
	src/utility/omSystemInformation.h \
	src/utility/omThreadPool.hpp \
	src/utility/omThreadPoolManager.h \
	src/utility/omTimer.hpp \
	src/utility/segmentDataWrapper.hpp \
	src/utility/segmentationDataWrapper.hpp \
	src/utility/setUtilities.h \
	src/utility/sortHelpers.h \
	src/view2d/om2dPreferences.hpp \
	src/view2d/omBrushOpp.hpp \
	src/view2d/omBrushOppCircle.hpp \
	src/view2d/omBrushOppLine.hpp \
	src/view2d/omBrushPaint.hpp \
	src/view2d/omBrushPaintCircle.hpp \
	src/view2d/omBrushPaintLine.hpp \
	src/view2d/omBrushPaintLineTask.hpp \
	src/view2d/omBrushSelect.hpp \
	src/view2d/omBrushSelectCircle.hpp \
	src/view2d/omBrushSelectLine.hpp \
	src/view2d/omBrushSelectLineTask.hpp \
	src/view2d/omDisplayInfo.hpp \
	src/view2d/omFillTool.hpp \
	src/view2d/omGuiHelpers.hpp \
	src/view2d/omKeyEvents.hpp \
	src/view2d/omMouseEventMove.hpp \
	src/view2d/omMouseEventPress.hpp \
	src/view2d/omMouseEventRelease.hpp \
	src/view2d/omMouseEventState.hpp \
	src/view2d/omMouseEventUtils.hpp \
	src/view2d/omMouseEventWheel.hpp \
	src/view2d/omMouseEvents.hpp \
	src/view2d/omOnScreenTileCoords.h \
	src/view2d/omPointsInCircle.hpp \
	src/view2d/omScreenPainter.hpp \
	src/view2d/omScreenShotSaver.hpp \
	src/view2d/omSliceCache.hpp \
	src/view2d/omTileDrawer.h \
	src/view2d/omView2d.h \
	src/view2d/omView2dConverters.hpp \
	src/view2d/omView2dCore.h \
	src/view2d/omView2dEvents.hpp \
	src/view2d/omView2dKeyPressEventListener.h \
	src/view2d/omView2dManager.hpp \
	src/view2d/omView2dManagerImpl.hpp \
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
	src/viewGroup/omSplitting.hpp \
	src/viewGroup/omViewGroupState.h \
	src/viewGroup/omZoomLevel.hpp \
	src/volume/build/omBuildAffinityChannel.hpp \
	src/volume/build/omBuildChannel.hpp \
	src/volume/build/omBuildSegmentation.hpp \
	src/volume/build/omBuildVolumes.hpp \
	src/volume/build/omCompactVolValues.hpp \
	src/volume/build/omDataCopierBase.hpp \
	src/volume/build/omDataCopierHdf5.hpp \
	src/volume/build/omDataCopierHdf5Task.hpp \
	src/volume/build/omDataCopierImages.hpp \
	src/volume/build/omDownsampler.hpp \
	src/volume/build/omDownsamplerTypes.hpp \
	src/volume/build/omDownsamplerVoxelTask.hpp \
	src/volume/build/omLoadImage.h \
	src/volume/build/omMSTImportHdf5.hpp \
	src/volume/build/omMSTImportWatershed.hpp \
	src/volume/build/omProcessSegmentationChunk.hpp \
	src/volume/build/omVolumeAllocater.hpp \
	src/volume/build/omVolumeBuilder.hpp \
	src/volume/build/omVolumeBuilderBase.hpp \
	src/volume/build/omVolumeBuilderHdf5.hpp \
	src/volume/build/omVolumeBuilderImages.hpp \
	src/volume/build/omVolumeBuilderWatershed.hpp \
	src/volume/build/omVolumeProcessor.h \
	src/volume/build/omWatershedMetadata.hpp \
	src/volume/io/omChunkOffset.hpp \
	src/volume/io/omMemMappedVolume.h \
	src/volume/io/omMemMappedVolumeImpl.hpp \
	src/volume/io/omVolumeData.h \
	src/volume/omAffinityChannel.h \
	src/volume/omAffinityGraph.h \
	src/volume/omChannel.h \
	src/volume/omChannelImpl.h \
	src/volume/omCompareVolumes.hpp \
	src/volume/omFilter2d.h \
	src/volume/omFilter2dManager.h \
	src/volume/omMipVolume.h \
	src/volume/omSegmentation.h \
	src/volume/omUpdateBoundingBoxes.h \
	src/volume/omVolCoords.hpp \
	src/volume/omVolCoordsMipped.hpp \
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
	src/actions/details/omSegmentSplitAction.cpp \
	src/actions/details/omSegmentUncertainAction.cpp \
	src/actions/details/omSegmentValidateAction.cpp \
	src/actions/details/omVoxelSetValueAction.cpp \
	src/actions/io/omActionOperators.cpp \
	src/actions/io/omActionReplayer.cpp \
	src/actions/omActions.cpp \
	src/chunks/details/omPtrToChunkDataMemMapVol.cpp \
	src/chunks/omChunk.cpp \
	src/chunks/omChunkCoord.cpp \
	src/chunks/omChunkData.cpp \
	src/chunks/omSegChunk.cpp \
	src/chunks/omSegChunkData.cpp \
	src/common/omCommon.cpp \
	src/common/omGl.cpp \
	src/datalayer/archive/omDataArchiveBoost.cpp \
	src/datalayer/archive/omDataArchiveProject.cpp \
	src/datalayer/archive/omDataArchiveSegment.cpp \
	src/datalayer/archive/omDataArchiveWrappers.cpp \
	src/datalayer/archive/omMetadataSegmentation.cpp \
	src/datalayer/archive/omMipVolumeArchive.cpp \
	src/datalayer/hdf5/omHdf5.cpp \
	src/datalayer/hdf5/omHdf5FileUtils.cpp \
	src/datalayer/hdf5/omHdf5Impl.cpp \
	src/datalayer/hdf5/omHdf5LowLevel.cpp \
	src/datalayer/hdf5/omHdf5Utils.cpp \
	src/datalayer/omDataPaths.cpp \
	src/events/omEvent.cpp \
	src/events/omEventManager.cpp \
	src/events/omPreferenceEvent.cpp \
	src/events/omProgressEvent.cpp \
	src/events/omSegmentEvent.cpp \
	src/events/omToolModeEvent.cpp \
	src/events/omView3dEvent.cpp \
	src/events/omViewEvent.cpp \
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
	src/project/omAffinityGraphManager.cpp \
	src/project/omChannelManager.cpp \
	src/project/omProject.cpp \
	src/project/omProjectGlobals.cpp \
	src/project/omProjectVolumes.cpp \
	src/project/omSegmentationManager.cpp \
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
	src/system/omAppState.cpp \
	src/system/omEvents.cpp \
	src/system/omGroups.cpp \
	src/system/omPreferenceDefinitions.cpp \
	src/system/omStateManager.cpp \
	src/system/omStateManagerImpl.cpp \
	src/system/templatedClasses.cpp \
	src/tiles/cache/omTileCache.cpp \
	src/tiles/omTextureID.cpp \
	src/tiles/omTile.cpp \
	src/tiles/omTileCoord.cpp \
	src/tiles/omTileDumper.cpp \
	src/tiles/omTilePreFetcher.cpp \
	src/tiles/omTilePreFetcherTask.cpp \
	src/utility/channelDataWrapper.cpp \
	src/utility/omFileHelpers.cpp \
	src/utility/omSystemInformation.cpp \
	src/utility/omThreadPoolManager.cpp \
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
	src/volume/build/omVolumeProcessor.cpp \
	src/volume/io/omMemMappedVolume.cpp \
	src/volume/io/omVolumeData.cpp \
	src/volume/omAffinityGraph.cpp \
	src/volume/omChannel.cpp \
	src/volume/omChannelImpl.cpp \
	src/volume/omFilter2d.cpp \
	src/volume/omFilter2dManager.cpp \
	src/volume/omMipVolume.cpp \
	src/volume/omSegmentation.cpp \
	src/volume/omUpdateBoundingBoxes.cpp \
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
        message( old g++ found )
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

CONFIG(debug, debug|release) {
    message ( in debug mode; adding gstabs+ )
    QMAKE_CXXFLAGS += -gstabs+
    QMAKE_LFLAGS   += -gstabs+
}

# GCC Parallel Mode support
linux-g++ {
    system( g++ --version | grep -e "4.[3-9]" ) {
        message( assuming g++ Parallel Mode supported )
        QMAKE_CXXFLAGS += -DZI_USE_OPENMP -fopenmp
        QMAKE_LFLAGS   += -DZI_USE_OPENMP -fopenmp
    }
}

# so QT wont define any non-all-caps keywords
CONFIG += no_keywords

#### for profiling
#QMAKE_CXXFLAGS += -pg
#QMAKE_LFLAGS   += -pg

