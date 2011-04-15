QT += opengl network
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
	include/enum/enum.hpp \
	include/enum/enum/base.hpp \
	include/enum/enum/bitfield.hpp \
	include/enum/enum/iterator.hpp \
	include/enum/enum/macros.hpp \
	include/json_spirit_v4.03/json_spirit/json_spirit.h \
	include/json_spirit_v4.03/json_spirit/json_spirit_error_position.h \
	include/json_spirit_v4.03/json_spirit/json_spirit_reader.h \
	include/json_spirit_v4.03/json_spirit/json_spirit_reader_template.h \
	include/json_spirit_v4.03/json_spirit/json_spirit_stream_reader.h \
	include/json_spirit_v4.03/json_spirit/json_spirit_utils.h \
	include/json_spirit_v4.03/json_spirit/json_spirit_value.h \
	include/json_spirit_v4.03/json_spirit/json_spirit_writer.h \
	include/json_spirit_v4.03/json_spirit/json_spirit_writer_template.h \
	include/vmmlib/axisAlignedBoundingBox.h \
	include/vmmlib/details.hpp \
	include/vmmlib/exception.hpp \
	include/vmmlib/frustum.h \
	include/vmmlib/frustum.hpp \
	include/vmmlib/frustumCuller.h \
	include/vmmlib/frustumCuller.hpp \
	include/vmmlib/helperFunctions.h \
	include/vmmlib/jacobiSolver.h \
	include/vmmlib/jacobiSolver.hpp \
	include/vmmlib/lapack.hpp \
	include/vmmlib/lapack_includes.hpp \
	include/vmmlib/lapack_linear_least_squares.hpp \
	include/vmmlib/lapack_svd.hpp \
	include/vmmlib/lapack_types.hpp \
	include/vmmlib/linear_least_squares.hpp \
	include/vmmlib/matrix.hpp \
	include/vmmlib/matrix3.h \
	include/vmmlib/matrix4.h \
	include/vmmlib/matrix_functors.hpp \
	include/vmmlib/qr_decomposition.hpp \
	include/vmmlib/quaternion.h \
	include/vmmlib/quaternion.hpp \
	include/vmmlib/stringUtils.h \
	include/vmmlib/stringUtils.hpp \
	include/vmmlib/svd.h \
	include/vmmlib/svd.hpp \
	include/vmmlib/vector.hpp \
	include/vmmlib/vector2.h \
	include/vmmlib/vector3.h \
	include/vmmlib/vector4.h \
	include/vmmlib/visibility.h \
	include/vmmlib/visibility.hpp \
	include/vmmlib/vmmlib.h \
	include/vmmlib/vmmlib.hpp \
	include/vmmlib/vmmlib_config.hpp \
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
	src/actions/io/omActionDumper.h \
	src/actions/io/omActionLogger.hpp \
	src/actions/io/omActionLoggerTask.hpp \
	src/actions/io/omActionOperators.h \
	src/actions/io/omActionOperatorsText.h \
	src/actions/io/omActionReplayer.hpp \
	src/actions/io/omActionTypes.h \
	src/actions/omActions.h \
	src/actions/omActionsImpl.h \
	src/actions/omSelectSegmentParams.hpp \
	src/chunks/details/omPtrToChunkDataBase.hpp \
	src/chunks/details/omPtrToChunkDataMemMapVol.h \
	src/chunks/omChunk.h \
	src/chunks/omChunkCache.hpp \
	src/chunks/omChunkCoord.h \
	src/chunks/omChunkData.h \
	src/chunks/omChunkItemContainer.hpp \
	src/chunks/omChunkItemContainerMatrix.hpp \
	src/chunks/omChunkMipping.hpp \
	src/chunks/omChunkUtils.hpp \
	src/chunks/omRawChunk.hpp \
	src/chunks/omRawChunkMemMapped.hpp \
	src/chunks/omRawChunkSlicer.hpp \
	src/chunks/omSegChunk.h \
	src/chunks/omSegChunkData.h \
	src/chunks/omSegChunkDataVisitors.hpp \
	src/chunks/uniqueValues/omChunkUniqueValuesManager.hpp \
	src/chunks/uniqueValues/omChunkUniqueValuesPerThreshold.hpp \
	src/chunks/uniqueValues/omChunkUniqueValuesTypes.h \
	src/chunks/uniqueValues/omThresholdsInChunk.hpp \
	src/common/om.hpp \
	src/common/omBoost.h \
	src/common/omColors.h \
	src/common/omCommon.h \
	src/common/omContainer.hpp \
	src/common/omDebug.h \
	src/common/omException.h \
	src/common/omGl.h \
	src/common/omQt.h \
	src/common/omSet.hpp \
	src/common/omStd.h \
	src/common/omString.hpp \
	src/datalayer/archive/omDataArchiveBoost.h \
	src/datalayer/archive/omDataArchiveProject.h \
	src/datalayer/archive/omDataArchiveSegment.h \
	src/datalayer/archive/omDataArchiveStd.hpp \
	src/datalayer/archive/omDataArchiveWrappers.h \
	src/datalayer/archive/omGenericManagerArchive.hpp \
	src/datalayer/archive/omMetadataSegmentation.h \
	src/datalayer/archive/omMipVolumeArchive.h \
	src/datalayer/archive/omMipVolumeArchiveOld.h \
	src/datalayer/fs/omCompressedFile.h \
	src/datalayer/fs/omFile.hpp \
	src/datalayer/fs/omFileNames.hpp \
	src/datalayer/fs/omFileQT.hpp \
	src/datalayer/fs/omIOnDiskFile.h \
	src/datalayer/fs/omMemMapCompressedFile.hpp \
	src/datalayer/fs/omMemMappedFileQT.hpp \
	src/datalayer/fs/omMemMappedFileQTNew.hpp \
	src/datalayer/fs/omOnDiskBoostUnorderedMap.hpp \
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
	src/events/details/omEvent.h \
	src/events/details/omEventManager.h \
	src/events/details/omEventManagerImpl.hpp \
	src/events/details/omPreferenceEvent.h \
	src/events/details/omSegmentEvent.h \
	src/events/details/omToolModeEvent.h \
	src/events/details/omUserInterfaceEvent.h \
	src/events/details/omView3dEvent.h \
	src/events/details/omViewEvent.h \
	src/events/omEvents.h \
	src/gui/brushToolbox/brushToolbox.hpp \
	src/gui/brushToolbox/brushToolboxImpl.h \
	src/gui/cacheMonitorDialog.h \
	src/gui/cacheMonitorWidget.h \
	src/gui/groupsTable/dropdownMenuButton.hpp \
	src/gui/groupsTable/groupsTable.h \
	src/gui/guiUtils.hpp \
	src/gui/inspectors/chanInspector.h \
	src/gui/inspectors/filObjectInspector.h \
	src/gui/inspectors/inspectorProperties.h \
	src/gui/inspectors/segmentInspector.h \
	src/gui/inspectors/segmentation/buildPage/pageBuilder.hpp \
	src/gui/inspectors/segmentation/buildPage/sourceBox.hpp \
	src/gui/inspectors/segmentation/exportPage/buttons/addSegmentButton.h \
	src/gui/inspectors/segmentation/exportPage/buttons/exportButton.hpp \
	src/gui/inspectors/segmentation/exportPage/buttons/exportButtonRaw.hpp \
	src/gui/inspectors/segmentation/exportPage/buttons/exportMST.hpp \
	src/gui/inspectors/segmentation/exportPage/buttons/exportSegmentList.hpp \
	src/gui/inspectors/segmentation/exportPage/buttons/meshPreviewButton.hpp \
	src/gui/inspectors/segmentation/exportPage/buttons/segmentationInspectorButtons.hpp \
	src/gui/inspectors/segmentation/exportPage/pageExport.h \
	src/gui/inspectors/segmentation/pageSelector.hpp \
	src/gui/inspectors/segmentation/pagesWidget.h \
	src/gui/inspectors/segmentation/segmentationInspector.hpp \
	src/gui/inspectors/ui_chanInspector.h \
	src/gui/inspectors/volInspector.h \
	src/gui/mainWindow/inspectorWidget.h \
	src/gui/mainWindow/mainWindow.h \
	src/gui/menubar.h \
	src/gui/meshPreviewer/meshPreviewer.hpp \
	src/gui/meshPreviewer/previewButton.hpp \
	src/gui/meshPreviewer/scaleFactorLineEdit.hpp \
	src/gui/mstViewer.hpp \
	src/gui/omImageDialog.h \
	src/gui/preferences/checkboxHideCrosshairs.hpp \
	src/gui/preferences/localPreferences2d.h \
	src/gui/preferences/localPreferences3d.h \
	src/gui/preferences/localPreferencesMeshing.h \
	src/gui/preferences/localPreferencesSystem.h \
	src/gui/preferences/preferences.h \
	src/gui/preferences/preferences2d.h \
	src/gui/preferences/preferences3d.h \
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
	src/gui/segmentLists/omSegmentListWidget.h \
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
	src/gui/toolbars/mainToolbar/openSingleViewButton.hpp \
	src/gui/toolbars/mainToolbar/openViewGroupButton.hpp \
	src/gui/toolbars/mainToolbar/refreshVolumeButton.h \
	src/gui/toolbars/mainToolbar/saveButton.h \
	src/gui/toolbars/mainToolbar/showPaintTools.hpp \
	src/gui/toolbars/mainToolbar/toolButton.h \
	src/gui/toolbars/toolbarManager.h \
	src/gui/updateSegmentPropertiesImpl.hpp \
	src/gui/viewGroup/viewGroup.h \
	src/gui/viewGroup/viewGroupUtils.hpp \
	src/gui/viewGroup/viewGroupWidgetInfo.h \
	src/gui/widgets/omAskQuestion.hpp \
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
	src/gui/widgets/omTellInfo.hpp \
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
	src/mesh/omMesh.h \
	src/mesh/omMeshCoord.h \
	src/mesh/omMeshManager.h \
	src/mesh/omMeshManagers.hpp \
	src/mesh/omMeshParams.hpp \
	src/mesh/omMeshTypes.h \
	src/mesh/omVolumeCuller.h \
	src/mesh/ziMesher.hpp \
	src/network/client/omClient.hpp \
	src/network/json/omJson.h \
	src/network/json/omJsonTypes.hpp \
	src/network/omAssembleTilesIntoSlice.hpp \
	src/network/omExtractMesh.h \
	src/network/omJSONActions.hpp \
	src/network/omProcessRequestFromClient.h \
	src/network/omServiceObjects.hpp \
	src/network/server/omNetworkQT.hpp \
	src/network/server/omServer.h \
	src/network/server/omServerImpl.hpp \
	src/network/server/omServerImplThread.hpp \
	src/project/details/omAffinityGraphManager.h \
	src/project/details/omChannelManager.h \
	src/project/details/omProjectVolumes.h \
	src/project/details/omSegmentationManager.h \
	src/project/omProject.h \
	src/project/omProjectGlobals.h \
	src/project/omProjectImpl.hpp \
	src/segment/colorizer/omSegmentColorizer.h \
	src/segment/colorizer/omSegmentColorizerColorCache.hpp \
	src/segment/colorizer/omSegmentColorizerImpl.hpp \
	src/segment/colorizer/omSegmentColorizerTypes.h \
	src/segment/colorizer/omSegmentColors.hpp \
	src/segment/io/omMST.h \
	src/segment/io/omMSTold.h \
	src/segment/io/omMSTtypes.h \
	src/segment/io/omSegmentListTypePage.hpp \
	src/segment/io/omSegmentPage.hpp \
	src/segment/io/omSegmentPageConverter.hpp \
	src/segment/io/omSegmentPageObjects.hpp \
	src/segment/io/omSegmentPageV1.hpp \
	src/segment/io/omSegmentPageV2.hpp \
	src/segment/io/omSegmentPageV3.hpp \
	src/segment/io/omSegmentPageV4.hpp \
	src/segment/io/omSegmentPageVersion.hpp \
	src/segment/io/omUserEdges.hpp \
	src/segment/io/omValidGroupNum.hpp \
	src/segment/lists/omSegmentListByMRU.hpp \
	src/segment/lists/omSegmentListForGUI.hpp \
	src/segment/lists/omSegmentListGlobal.hpp \
	src/segment/lists/omSegmentListLowLevel.hpp \
	src/segment/lists/omSegmentLists.h \
	src/segment/lists/omSegmentListsTypes.hpp \
	src/segment/lowLevel/DynamicForestPool.hpp \
	src/segment/lowLevel/omDynamicForestCache.hpp \
	src/segment/lowLevel/omEnabledSegments.hpp \
	src/segment/lowLevel/omPagingPtrStore.h \
	src/segment/lowLevel/omSegmentChildren.hpp \
	src/segment/lowLevel/omSegmentGraph.h \
	src/segment/lowLevel/omSegmentGraphInitialLoad.hpp \
	src/segment/lowLevel/omSegmentLowLevelTypes.h \
	src/segment/lowLevel/omSegmentSelection.hpp \
	src/segment/lowLevel/omSegmentsImplLowLevel.h \
	src/segment/lowLevel/store/omCacheSegRootIDs.hpp \
	src/segment/lowLevel/store/omCacheSegStore.hpp \
	src/segment/lowLevel/store/omSegmentStore.hpp \
	src/segment/omFindCommonEdge.hpp \
	src/segment/omSegment.h \
	src/segment/omSegmentCenter.hpp \
	src/segment/omSegmentChildrenTypes.h \
	src/segment/omSegmentEdge.h \
	src/segment/omSegmentIterator.h \
	src/segment/omSegmentPointers.h \
	src/segment/omSegmentSearched.hpp \
	src/segment/omSegmentSelected.hpp \
	src/segment/omSegmentSelector.h \
	src/segment/omSegmentTypes.h \
	src/segment/omSegmentUncertain.hpp \
	src/segment/omSegmentUtils.hpp \
	src/segment/omSegmentValidation.hpp \
	src/segment/omSegments.h \
	src/segment/omSegmentsImpl.h \
	src/system/cache/omCacheBase.h \
	src/system/cache/omCacheGroup.h \
	src/system/cache/omCacheInfo.h \
	src/system/cache/omCacheManager.h \
	src/system/cache/omCacheManagerImpl.hpp \
	src/system/cache/omCacheObjects.hpp \
	src/system/cache/omGetSetCache.hpp \
	src/system/cache/omLockedCacheObjects.hpp \
	src/system/cache/omMeshCache.h \
	src/system/cache/omThreadedCache.h \
	src/system/cache/omVolSliceCache.hpp \
	src/system/cache/omVolSliceCacheTypes.hpp \
	src/system/omAlphaVegasMode.hpp \
	src/system/omAppState.hpp \
	src/system/omConnect.hpp \
	src/system/omGarbage.h \
	src/system/omGenericManager.hpp \
	src/system/omGlobalKeyPress.hpp \
	src/system/omGroup.h \
	src/system/omGroups.h \
	src/system/omLocalPreferences.hpp \
	src/system/omManageableObject.h \
	src/system/omPreferenceDefinitions.h \
	src/system/omPreferences.h \
	src/system/omQTApp.hpp \
	src/system/omStateManager.h \
	src/system/omStateManagerImpl.hpp \
	src/system/omUndoStack.hpp \
	src/threads/omTaskManager.hpp \
	src/threads/omTaskManagerContainer.hpp \
	src/threads/omTaskManagerImpl.hpp \
	src/threads/omThreadPool.hpp \
	src/threads/omThreadPoolBatched.hpp \
	src/threads/omThreadPoolByMipLevel.hpp \
	src/threads/omThreadPoolManager.h \
	src/tiles/cache/omTileCache.h \
	src/tiles/cache/omTileCacheChannel.hpp \
	src/tiles/cache/omTileCacheEventListener.hpp \
	src/tiles/cache/omTileCacheImpl.hpp \
	src/tiles/cache/omTileCacheSegmentation.hpp \
	src/tiles/omChannelTileFilter.hpp \
	src/tiles/omPooledTile.hpp \
	src/tiles/omTextureID.h \
	src/tiles/omTile.h \
	src/tiles/omTileCoord.h \
	src/tiles/omTileDumper.hpp \
	src/tiles/omTileFilters.hpp \
	src/tiles/omTileImplTypes.hpp \
	src/tiles/omTilePool.hpp \
	src/tiles/omTilePools.hpp \
	src/tiles/omTilePreFetcher.h \
	src/tiles/omTilePreFetcherTask.hpp \
	src/tiles/omTileTypes.hpp \
	src/utility/affinityGraphDataWrapper.hpp \
	src/utility/channelDataWrapper.hpp \
	src/utility/dataWrapperContainer.hpp \
	src/utility/dataWrappers.h \
	src/utility/filterDataWrapper.hpp \
	src/utility/fuzzyStdObjs.hpp \
	src/utility/image/bits/omImage_traits.hpp \
	src/utility/image/omImage.hpp \
	src/utility/localPrefFiles.h \
	src/utility/localPrefFilesImpl.hpp \
	src/utility/omChunkVoxelWalker.hpp \
	src/utility/omColorUtils.hpp \
	src/utility/omCopyFirstN.hpp \
	src/utility/omDataTime.hpp \
	src/utility/omFileHelpers.h \
	src/utility/omFileLogger.hpp \
	src/utility/omLockedObjects.h \
	src/utility/omLockedPODs.hpp \
	src/utility/omPrimeNumbers.hpp \
	src/utility/omRand.hpp \
	src/utility/omRandColorFile.hpp \
	src/utility/omSharedPtr.hpp \
	src/utility/omSimpleProgress.hpp \
	src/utility/omSmartPtr.hpp \
	src/utility/omStringHelpers.h \
	src/utility/omSystemInformation.h \
	src/utility/omTempFile.hpp \
	src/utility/omTimer.hpp \
	src/utility/omUUID.hpp \
	src/utility/segmentDataWrapper.hpp \
	src/utility/segmentationDataWrapper.hpp \
	src/utility/setUtilities.h \
	src/utility/sortHelpers.h \
	src/view2d/brush/omBrushErase.hpp \
	src/view2d/brush/omBrushEraseCircle.hpp \
	src/view2d/brush/omBrushEraseLine.hpp \
	src/view2d/brush/omBrushEraseLineTask.hpp \
	src/view2d/brush/omBrushEraseUtils.hpp \
	src/view2d/brush/omBrushOppCircle.hpp \
	src/view2d/brush/omBrushOppInfo.hpp \
	src/view2d/brush/omBrushOppLine.hpp \
	src/view2d/brush/omBrushOppTypes.h \
	src/view2d/brush/omBrushOppUtils.hpp \
	src/view2d/brush/omBrushPaint.hpp \
	src/view2d/brush/omBrushPaintCircle.hpp \
	src/view2d/brush/omBrushPaintLine.hpp \
	src/view2d/brush/omBrushPaintLineTask.hpp \
	src/view2d/brush/omBrushPaintUtils.hpp \
	src/view2d/brush/omBrushSelect.hpp \
	src/view2d/brush/omBrushSelectCircle.hpp \
	src/view2d/brush/omBrushSelectLine.hpp \
	src/view2d/brush/omBrushSelectLineTask.hpp \
	src/view2d/brush/omBrushSelectUtils.hpp \
	src/view2d/brush/omChunksAndPts.hpp \
	src/view2d/om2dPreferences.hpp \
	src/view2d/omBlockingGetTiles.hpp \
	src/view2d/omCalcTileCoordsDownsampled.hpp \
	src/view2d/omDisplayInfo.hpp \
	src/view2d/omFillTool.hpp \
	src/view2d/omKeyEvents.hpp \
	src/view2d/omMouseEventMove.hpp \
	src/view2d/omMouseEventPress.hpp \
	src/view2d/omMouseEventRelease.hpp \
	src/view2d/omMouseEventState.hpp \
	src/view2d/omMouseEventUtils.hpp \
	src/view2d/omMouseEventWheel.hpp \
	src/view2d/omMouseEvents.hpp \
	src/view2d/omOnScreenTileCoords.h \
	src/view2d/omOpenGLTileDrawer.hpp \
	src/view2d/omOpenGLUtils.h \
	src/view2d/omPointsInCircle.hpp \
	src/view2d/omScreenPainter.hpp \
	src/view2d/omSliceCache.hpp \
	src/view2d/omTileDrawer.hpp \
	src/view2d/omView2d.h \
	src/view2d/omView2dConverters.hpp \
	src/view2d/omView2dCore.h \
	src/view2d/omView2dEvents.hpp \
	src/view2d/omView2dManager.hpp \
	src/view2d/omView2dManagerImpl.hpp \
	src/view2d/omView2dState.hpp \
	src/view2d/omView2dVolLocation.hpp \
	src/view2d/omView2dZoom.hpp \
	src/view3d/om3dPreferences.hpp \
	src/view3d/omCamera.h \
	src/view3d/omCameraMovement.h \
	src/view3d/omMacOSXgestures.hpp \
	src/view3d/omView3d.h \
	src/view3d/omView3dUi.h \
	src/view3d/omView3dWidget.h \
	src/view3d/widgets/omChunkExtentWidget.h \
	src/view3d/widgets/omInfoWidget.h \
	src/view3d/widgets/omPercDone.hpp \
	src/view3d/widgets/omSelectionWidget.h \
	src/view3d/widgets/omViewBoxWidget.h \
	src/view3d/widgets/omVolumeAxisWidget.h \
	src/viewGroup/omBrushSize.hpp \
	src/viewGroup/omColorizers.hpp \
	src/viewGroup/omCutting.hpp \
	src/viewGroup/omSplitting.hpp \
	src/viewGroup/omViewGroupState.h \
	src/viewGroup/omViewGroupView2dState.hpp \
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
	tests/onDiskTests.hpp \
	tests/segment/mockSegments.hpp \
	tests/segment/omSegmentListBySizeTests.hpp \
	tests/segment/segmentTests.hpp \
	tests/segment/segmentTests1.hpp \
	tests/testUtils.hpp \
	tests/tests.hpp \
	tests/tiles/omTilePoolTests.hpp \
	tests/utility/stringHelpersTest.h

SOURCES +=  \
	include/json_spirit_v4.03/json_spirit/json_spirit_reader.cpp \
	include/json_spirit_v4.03/json_spirit/json_spirit_value.cpp \
	include/json_spirit_v4.03/json_spirit/json_spirit_writer.cpp \
	lib/strnatcmp.cpp \
	src/actions/details/omSegmentSplitAction.cpp \
	src/actions/details/omSegmentUncertainAction.cpp \
	src/actions/details/omSegmentValidateAction.cpp \
	src/actions/details/omVoxelSetValueAction.cpp \
	src/actions/io/omActionDumper.cpp \
	src/actions/io/omActionOperators.cpp \
	src/actions/io/omActionOperatorsText.cpp \
	src/actions/io/omActionReplayer.cpp \
	src/actions/io/omActionTypes.cpp \
	src/actions/omActions.cpp \
	src/actions/omActionsImpl.cpp \
	src/chunks/details/omPtrToChunkDataMemMapVol.cpp \
	src/chunks/omChunk.cpp \
	src/chunks/omChunkCoord.cpp \
	src/chunks/omChunkData.cpp \
	src/chunks/omSegChunk.cpp \
	src/chunks/omSegChunkData.cpp \
	src/common/omColors.cpp \
	src/common/omCommon.cpp \
	src/common/omGl.cpp \
	src/common/omQt.cpp \
	src/datalayer/archive/omDataArchiveBoost.cpp \
	src/datalayer/archive/omDataArchiveProject.cpp \
	src/datalayer/archive/omDataArchiveSegment.cpp \
	src/datalayer/archive/omDataArchiveWrappers.cpp \
	src/datalayer/archive/omMetadataSegmentation.cpp \
	src/datalayer/archive/omMipVolumeArchive.cpp \
	src/datalayer/fs/omCompressedFile.cpp \
	src/datalayer/fs/omFile.cpp \
	src/datalayer/hdf5/omHdf5.cpp \
	src/datalayer/hdf5/omHdf5FileUtils.cpp \
	src/datalayer/hdf5/omHdf5Impl.cpp \
	src/datalayer/hdf5/omHdf5LowLevel.cpp \
	src/datalayer/hdf5/omHdf5Utils.cpp \
	src/datalayer/omDataPaths.cpp \
	src/events/details/omEvent.cpp \
	src/events/details/omEventManager.cpp \
	src/events/details/omPreferenceEvent.cpp \
	src/events/details/omSegmentEvent.cpp \
	src/events/details/omToolModeEvent.cpp \
	src/events/details/omUserInterfaceEvent.cpp \
	src/events/details/omView3dEvent.cpp \
	src/events/details/omViewEvent.cpp \
	src/events/omEvents.cpp \
	src/gui/brushToolbox/brushToolboxImpl.cpp \
	src/gui/cacheMonitorDialog.cpp \
	src/gui/cacheMonitorWidget.cpp \
	src/gui/groupsTable/groupsTable.cpp \
	src/gui/inspectors/chanInspector.cpp \
	src/gui/inspectors/filObjectInspector.cpp \
	src/gui/inspectors/inspectorProperties.cpp \
	src/gui/inspectors/segmentInspector.cpp \
	src/gui/inspectors/segmentation/exportPage/pageExport.cpp \
	src/gui/inspectors/segmentation/pagesWidget.cpp \
	src/gui/mainWindow/inspectorWidget.cpp \
	src/gui/mainWindow/mainWindow.cpp \
	src/gui/menubar.cpp \
	src/gui/meshPreviewer/meshPreviewer.cpp \
	src/gui/mstViewer.cpp \
	src/gui/preferences/localPreferences2d.cpp \
	src/gui/preferences/localPreferences3d.cpp \
	src/gui/preferences/localPreferencesMeshing.cpp \
	src/gui/preferences/localPreferencesSystem.cpp \
	src/gui/preferences/preferences.cpp \
	src/gui/preferences/preferences2d.cpp \
	src/gui/preferences/preferences3d.cpp \
	src/gui/recentFileList.cpp \
	src/gui/segmentLists/details/segmentListBase.cpp \
	src/gui/segmentLists/elementListBox.cpp \
	src/gui/segmentLists/omSegmentListWidget.cpp \
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
	src/gui/toolbars/mainToolbar/refreshVolumeButton.cpp \
	src/gui/toolbars/mainToolbar/saveButton.cpp \
	src/gui/toolbars/mainToolbar/toolButton.cpp \
	src/gui/toolbars/toolbarManager.cpp \
	src/gui/viewGroup/viewGroup.cpp \
	src/gui/viewGroup/viewGroupUtils.cpp \
	src/gui/widgets/omCursors.cpp \
	src/gui/widgets/omGroupListWidget.cpp \
	src/gui/widgets/omSegmentContextMenu.cpp \
	src/headless/headless.cpp \
	src/main.cpp \
	src/mesh/drawer/omMeshDrawer.cpp \
	src/mesh/drawer/omMeshSegmentListTask.cpp \
	src/mesh/io/v2/chunk/omMeshChunkAllocTable.cpp \
	src/mesh/omMesh.cpp \
	src/mesh/omMeshCoord.cpp \
	src/mesh/omMeshManager.cpp \
	src/mesh/omVolumeCuller.cpp \
	src/network/omAssembleTilesIntoSlice.cpp \
	src/network/omExtractMesh.cpp \
	src/network/omProcessRequestFromClient.cpp \
	src/network/omServiceObjects.cpp \
	src/network/server/omNetworkQT.cpp \
	src/network/server/omServer.cpp \
	src/network/server/omServerImplThread.cpp \
	src/project/details/omAffinityGraphManager.cpp \
	src/project/details/omChannelManager.cpp \
	src/project/details/omProjectVolumes.cpp \
	src/project/details/omSegmentationManager.cpp \
	src/project/omProject.cpp \
	src/project/omProjectGlobals.cpp \
	src/segment/colorizer/omSegmentColorizer.cpp \
	src/segment/io/omMST.cpp \
	src/segment/io/omMSTold.cpp \
	src/segment/lists/omSegmentListByMRU.cpp \
	src/segment/lists/omSegmentLists.cpp \
	src/segment/lists/omSegmentListsTypes.cpp \
	src/segment/lowLevel/omPagingPtrStore.cpp \
	src/segment/lowLevel/omSegmentGraph.cpp \
	src/segment/lowLevel/omSegmentsImplLowLevel.cpp \
	src/segment/lowLevel/store/omSegmentStore.cpp \
	src/segment/omSegment.cpp \
	src/segment/omSegmentIterator.cpp \
	src/segment/omSegmentSelector.cpp \
	src/segment/omSegments.cpp \
	src/segment/omSegmentsImpl.cpp \
	src/system/cache/omCacheGroup.cpp \
	src/system/cache/omCacheManager.cpp \
	src/system/cache/omMeshCache.cpp \
	src/system/omAppState.cpp \
	src/system/omGroups.cpp \
	src/system/omPreferenceDefinitions.cpp \
	src/system/omStateManager.cpp \
	src/system/omStateManagerImpl.cpp \
	src/threads/omThreadPoolManager.cpp \
	src/tiles/cache/omTileCache.cpp \
	src/tiles/cache/omTileCacheEventListener.cpp \
	src/tiles/omTextureID.cpp \
	src/tiles/omTile.cpp \
	src/tiles/omTileCoord.cpp \
	src/tiles/omTileDumper.cpp \
	src/tiles/omTileImplTypes.cpp \
	src/tiles/omTilePreFetcher.cpp \
	src/tiles/omTilePreFetcherTask.cpp \
	src/utility/channelDataWrapper.cpp \
	src/utility/omFileHelpers.cpp \
	src/utility/omPrimeNumbers.cpp \
	src/utility/omSystemInformation.cpp \
	src/view2d/brush/omBrushOppTypes.cpp \
	src/view2d/omOnScreenTileCoords.cpp \
	src/view2d/omOpenGLUtils.cpp \
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
INCLUDEPATH += ../external/libs/libjpeg/include
INCLUDEPATH +=  include/json_spirit_v4.03/json_spirit

#### Windows
win32 {
   INCLUDEPATH += c:/hdf5lib/include C:/mygl
   LIBS += /omni/external/srcs/hdf5-1.8.4-patch1/src/.libs/libhdf5.a
   LIBS += -lgdi32
} else {
#### Linux or MacOS
   INCLUDEPATH +=  ../external/libs/HDF5/include
   LIBS += ../external/libs/HDF5/lib/libhdf5.a
   LIBS += ../external/libs/libjpeg/lib/libjpeg.a
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
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS += -DNDEBUG  -fno-strict-aliasing -O3 -fno-omit-frame-pointer
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

# so QT wont define any non-all-caps keywords
CONFIG += no_keywords

#### for profiling
#QMAKE_CXXFLAGS += -pg
#QMAKE_LFLAGS   += -pg

!exists(gprofile) {
  CONFIG(debug, debug|release) {
      message ( in debug mode ; adding gstabs+ )
      QMAKE_CXXFLAGS += -gstabs+
      QMAKE_LFLAGS   += -gstabs+
  }
} else {
  QMAKE_LFLAGS   += -lprofiler
  QMAKE_CXXFLAGS += -fno-omit-frame-pointer
}

!exists(../external/libs/libjpeg/lib/libjpeg.a){
    error(please run 'bootstrap.pl 4' to install libjpeg)
}

#### Boost
exists(../external/libs/Boost) {
     INCLUDEPATH += ../external/libs/Boost/include

     LIBS += ../external/libs/Boost/lib/libboost_filesystem.a
     LIBS += ../external/libs/Boost/lib/libboost_iostreams.a
     LIBS += ../external/libs/Boost/lib/libboost_system.a
     LIBS += ../external/libs/Boost/lib/libboost_thread.a

     QMAKE_CXXFLAGS += -DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION
     QMAKE_CXXFLAGS += -DBOOST_SPIRIT_THREADSAFE
     QMAKE_CXXFLAGS += -DBOOST_SYSTEM_NO_DEPRECATED
     QMAKE_CXXFLAGS += -DBOOST_FILESYSTEM_VERSION=3
     QMAKE_CXXFLAGS += -DBOOST_FILESYSTEM_NO_DEPRECATED

}else {
    error(please run 'bootstrap.pl  2' to install Boost)
}
