######################################################################
# Automatically generated by qmake (2.01a) Fri Apr 23 13:58:43 2010
######################################################################

QT += opengl network
TEMPLATE = app
TARGET = 
DEPENDPATH += . \
              lib \
              src \
              CMakeFiles/CompilerIdC \
              CMakeFiles/CompilerIdCXX \
              include/vmmlib \
              inst/gui \
              src/common \
              src/gui \
              src/mesh \
              src/project \
              src/segment \
              src/system \
              src/utility \
              src/view2d \
              src/view3d \
              src/volume \
              src/voxel \
              src/gui/inspectors \
              src/gui/preferences \
              src/system/events \
              src/system/viewGroup \
              src/view3d/widgets \
              src/segment/actions/edit \
              src/segment/actions/segment \
              src/segment/actions/voxel

INCLUDEPATH += . \
               lib \
               src/common \
               src/gui \
               src/volume \
               src/system \
               src/system/events \
               src/view2d \
               src/mesh \
               src/utility \
               src/voxel \
               src/segment \
               src/gui/preferences \
               src/project \
               src/gui/inspectors \
               src/segment/actions/segment \
               src/view3d \
               src/segment/actions/voxel \
               src/segment/actions/edit \
               src/view3d/widgets \
               src/system/viewGroup

# Input
HEADERS += lib/strnatcmp.h \
           src/mainpage.h \
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
           src/common/omCommon.h \
           src/common/omDebug.h \
           src/common/omException.h \
           src/common/omGl.h \
           src/common/omStd.h \
           src/common/omVtk.h \
           src/gui/cacheMonitorDialog.h \
           src/gui/cacheMonitorWidget.h \
           src/system/OmCacheInfo.h \
           src/gui/elementListBox.h \
           src/gui/guiUtils.h \
           src/gui/headless.h \
           src/gui/mainwindow.h \
           src/gui/toolbars/toolbarManager.h \
           src/gui/toolbars/navAndEditToolBars.h \
           src/gui/toolbars/dendToolbar.h \
           src/gui/menubar.h \
           src/gui/myInspectorWidget.h \
           src/gui/recentFileList.h \
           src/gui/searchInspector.h \
           src/gui/segmentListAll.h \
           src/gui/segmentListBase.h \
           src/gui/segmentListValid.h \
           src/gui/segmentListRecent.h \
           src/gui/viewGroup.h \
           src/gui/viewGroupWidgetInfo.h \
           src/mesh/meshingChunkThread.h \
           src/mesh/meshingChunkThreadManager.h \
           src/mesh/meshingManager.h \
           src/mesh/omMesher.h \
           src/mesh/omMeshSource.h \
           src/mesh/omMeshTypes.h \
           src/mesh/omMipMesh.h \
           src/mesh/omMeshDrawer.h \
           src/mesh/omMipMeshCoord.h \
           src/mesh/omMipMeshManager.h \
           src/project/omProject.h \
           src/segment/DynamicTree.h \
           src/segment/DynamicTreeContainer.h \
           src/segment/omSegment.h \
           src/segment/omSegmentEdge.h \
           src/segment/omSegmentPointers.h \
           src/segment/omSegmentQueue.h \
           src/segment/omSegmentCache.h \
           src/segment/omSegmentCacheImpl.h \
           src/segment/omSegmentIterator.h \
           src/segment/omSegmentColorizer.h \
           src/segment/omSegmentSelector.h \
           src/segment/helpers/omSegmentListBySize.h \
           src/segment/helpers/omSegmentListByMRU.h \
           src/segment/lowLevel/omPagingStore.h \
           src/segment/lowLevel/omSegmentCacheImplLowLevel.h \
           src/system/omAction.h \
           src/system/omAnimate.h \
           src/system/omBuildVolumes.h \
           src/system/omBuildChannel.h \
           src/system/omBuildSegmentation.h \
           src/system/omCacheableBase.h \
           src/system/omCacheBase.h \
           src/system/omCacheManager.h \
           src/system/omEvent.h \
           src/system/omEventManager.h \
           src/system/omFetchingThread.h \
           src/system/omGarbage.h \
           src/system/omGenericManager.h \
           src/system/omGroups.h \
           src/system/omGroup.h \
           src/system/omLocalPreferences.h \
           src/system/omManageableObject.h \
           src/system/omPreferenceDefinitions.h \
           src/system/omPreferences.h \
           src/system/omProjectData.h \
           src/system/omStateManager.h \
           src/system/omThreadedCache.h \
           src/utility/dataWrappers.h \
           src/utility/fileHelpers.h \
           src/utility/localPrefFiles.h \
           src/datalayer/archive/omDataArchiveCoords.h \
           src/datalayer/archive/omDataArchiveMipChunk.h \
           src/datalayer/archive/omDataArchiveProject.h \
           src/datalayer/archive/omDataArchiveQT.h \
           src/datalayer/archive/omDataArchiveSegment.h \
           src/datalayer/archive/omDataArchiveVmml.h \
           src/datalayer/archive/omDataArchiveBoost.h \
           src/datalayer/omDataPaths.h \
           src/datalayer/omDataLayer.h \
           src/datalayer/omDataReader.h \
           src/datalayer/omDataWrapper.h \
           src/datalayer/omDataWriter.h \
           src/datalayer/omDummyWriter.h \
           src/datalayer/hdf5/omHdf5.h \
           src/datalayer/hdf5/omHdf5LowLevel.h \
           src/datalayer/hdf5/omHdf5LowLevelWrappersManualOpenClose.h \
           src/datalayer/hdf5/omHdf5Manager.h \
           src/datalayer/omDataPath.h \
           src/datalayer/hdf5/omHdf5Reader.h \
           src/datalayer/hdf5/omHdf5Writer.h \
           src/utility/omImageDataIo.h \
           src/utility/omSystemInformation.h \
           src/utility/setUtilities.h \
           src/utility/sortHelpers.h \
           src/utility/stringHelpers.h \
           src/view2d/drawable.h \
           src/view2d/omTextureID.h \
           src/view2d/omThreadedCachingTile.h \
           src/view2d/omCachingThreadedCachingTile.h \
           src/view2d/omTile.h \
           src/view2d/omTileCoord.h \
           src/view2d/omView2d.h \
           src/view2d/omView2dImpl.h \
           src/view3d/omCamera.h \
           src/view3d/omCameraMovement.h \
           src/view3d/omView3d.h \
           src/view3d/omView3dUi.h \
           src/view3d/omView3dWidget.h \
           src/volume/omChannel.h \
           src/volume/omDrawOptions.h \
           src/volume/omFilter2d.h \
           src/volume/omFilter2dManager.h \
           src/volume/omMipChunk.h \
           src/volume/omMipChunkPtr.h \
           src/volume/omMipChunkCoord.h \
           src/volume/omMipVolume.h \
           src/volume/omSegmentation.h \
           src/volume/omSegmentationChunkCoord.h \
           src/volume/omSimpleChunk.h \
           src/volume/omSimpleChunkThreadedCache.h \
           src/volume/omVolume.h \
           src/volume/omVolumeCuller.h \
           src/voxel/omMipSegmentDataCoord.h \
           src/voxel/omMipVoxelation.h \
           src/voxel/omMipVoxelationManager.h \
           src/gui/inspectors/chanInspector.h \
           src/gui/inspectors/filObjectInspector.h \
           src/gui/inspectors/inspectorProperties.h \
           src/gui/inspectors/segInspector.h \
           src/gui/inspectors/volInspector.h \
           src/gui/inspectors/segObjectInspector.h \
           src/gui/inspectors/ui_chanInspector.h \
           src/gui/preferences/localPreferences2d.h \
           src/gui/preferences/localPreferences3d.h \
           src/gui/preferences/localPreferencesMeshing.h \
           src/gui/preferences/localPreferencesSystem.h \
           src/gui/preferences/preferences.h \
           src/gui/preferences/preferences2d.h \
           src/gui/preferences/preferences3d.h \
           src/gui/preferences/preferencesMesh.h \
           src/gui/preferences/ui_preferences3d.h \
           src/gui/omSegmentListWidget.h \
           src/gui/omSegmentContextMenu.h \
           src/system/events/omPreferenceEvent.h \
           src/system/events/omProgressEvent.h \
           src/system/events/omSegmentEvent.h \
           src/system/events/omSystemModeEvent.h \
           src/system/events/omToolModeEvent.h \
           src/system/events/omView3dEvent.h \
           src/system/events/omViewEvent.h \
           src/system/events/omVoxelEvent.h \
           src/system/viewGroup/omViewGroupState.h \
           src/view3d/widgets/omChunkExtentWidget.h \
           src/view3d/widgets/omInfoWidget.h \
           src/view3d/widgets/omSelectionWidget.h \
           src/view3d/widgets/omViewBoxWidget.h \
           src/view3d/widgets/omVolumeAxisWidget.h \
           src/segment/actions/edit/omEditSelectionSetAction.h \
           src/segment/actions/segment/omSegmentSelectAction.h \
           src/segment/actions/segment/omSegmentJoinAction.h \
           src/segment/actions/segment/omSegmentSplitAction.h \
           src/segment/actions/omSegmentEditor.h \
           src/segment/actions/voxel/omVoxelSelectionAction.h \
           src/segment/actions/voxel/omVoxelSelectionSetAction.h \
           src/segment/actions/voxel/omVoxelSetAction.h \
           src/segment/actions/voxel/omVoxelSetConnectedAction.h \
           src/segment/actions/voxel/omVoxelSetValueAction.h \
           tests/utility/stringHelpersTest.h

SOURCES += lib/strnatcmp.cpp \
           src/common/omCommon.cpp \
           src/common/omDebug.cpp \
           src/common/omException.cpp \
           src/common/omGl.cpp \
           src/common/omStd.cpp \
           src/gui/cacheMonitorDialog.cpp \
           src/gui/cacheMonitorWidget.cpp \
           src/system/OmCacheInfo.cpp \
           src/gui/elementListBox.cpp \
           src/gui/guiUtils.cpp \
           src/gui/headless.cpp \
           src/gui/main.cpp \
           src/gui/mainwindow.cpp \
           src/gui/toolbars/toolbarManager.cpp \
           src/gui/toolbars/navAndEditToolBars.cpp \
           src/gui/toolbars/dendToolbar.cpp \
           src/gui/menubar.cpp \
           src/gui/myInspectorWidget.cpp \
           src/gui/recentFileList.cpp \
           src/gui/segmentListAll.cpp \
           src/gui/segmentListBase.cpp \
           src/gui/segmentListValid.cpp \
           src/gui/segmentListRecent.cpp \
           src/gui/viewGroup.cpp \
           src/gui/omSegmentContextMenu.cpp \
           src/gui/omSegmentListWidget.cpp \
           src/mesh/meshingChunkThread.cpp \
           src/mesh/meshingChunkThreadManager.cpp \
           src/mesh/meshingManager.cpp \
           src/mesh/omMesher.cpp \
           src/mesh/omMeshSource.cpp \
           src/mesh/omMipMesh.cpp \
           src/mesh/omMeshDrawer.cpp \
           src/mesh/omMipMeshCoord.cpp \
           src/mesh/omMipMeshManager.cpp \
           src/project/omProject.cpp \
           src/segment/DynamicTree.cpp \
           src/segment/DynamicTreeContainer.cpp \
           src/segment/omSegment.cpp \
           src/segment/omSegmentQueue.cpp \
           src/segment/omSegmentCache.cpp \
           src/segment/omSegmentCacheImpl.cpp \
           src/segment/omSegmentIterator.cpp \
           src/segment/omSegmentColorizer.cpp \
           src/segment/omSegmentSelector.cpp \
           src/segment/helpers/omSegmentListBySize.cpp \
           src/segment/helpers/omSegmentListByMRU.cpp \
           src/segment/lowLevel/omPagingStore.cpp \
           src/segment/lowLevel/omSegmentCacheImplLowLevel.cpp \
           src/system/omAction.cpp \
           src/system/omAnimate.cpp \
           src/system/omBuildVolumes.cpp \
           src/system/omBuildChannel.cpp \
           src/system/omBuildSegmentation.cpp \
           src/system/omCacheableBase.cpp \
           src/system/omCacheManager.cpp \
           src/system/omEvent.cpp \
           src/system/omEventManager.cpp \
           src/system/omFetchingThread.cpp \
           src/system/omGarbage.cpp \
           src/system/omGenericManager.cpp \
           src/system/omLocalPreferences.cpp \
           src/system/omManageableObject.cpp \
           src/system/omPreferenceDefinitions.cpp \
           src/system/omPreferences.cpp \
           src/system/omProjectData.cpp \
           src/system/omStateManager.cpp \
           src/system/omThreadedCache.cpp \
           src/system/templatedClasses.cpp \
           src/utility/dataWrappers.cpp \
           src/utility/fileHelpers.cpp \
           src/utility/localPrefFiles.cpp \
           src/datalayer/archive/omDataArchiveCoords.cpp \
           src/datalayer/archive/omDataArchiveMipChunk.cpp \
           src/datalayer/archive/omDataArchiveProject.cpp \
           src/datalayer/archive/omDataArchiveQT.cpp \
           src/datalayer/archive/omDataArchiveSegment.cpp \
           src/datalayer/archive/omDataArchiveVmml.cpp \
           src/datalayer/archive/omDataArchiveBoost.cpp \
           src/datalayer/omDataPaths.cpp \
           src/datalayer/omDataLayer.cpp \
           src/datalayer/omDummyWriter.cpp \
           src/datalayer/hdf5/omHdf5.cpp \
           src/datalayer/hdf5/omHdf5LowLevel.cpp \
           src/datalayer/hdf5/omHdf5LowLevelWrappersManualOpenClose.cpp \
           src/datalayer/hdf5/omHdf5Manager.cpp \
           src/datalayer/omDataPath.cpp \
           src/datalayer/hdf5/omHdf5Reader.cpp \
           src/datalayer/hdf5/omHdf5Writer.cpp \
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
           src/view2d/omView2dImpl.cpp \
           src/view2d/omView2dConverters.cpp \
           src/view2d/omView2dEvent.cpp \
           src/view3d/omCamera.cpp \
           src/view3d/omCameraMovement.cpp \
           src/view3d/omView3d.cpp \
           src/view3d/omView3dUi.cpp \
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
           src/gui/inspectors/chanInspector.cpp \
           src/gui/inspectors/filObjectInspector.cpp \
           src/gui/inspectors/inspectorProperties.cpp \
           src/gui/inspectors/segInspector.cpp \
           src/gui/inspectors/volInspector.cpp \
           src/gui/inspectors/segObjectInspector.cpp \
           src/gui/preferences/localPreferences2d.cpp \
           src/gui/preferences/localPreferences3d.cpp \
           src/gui/preferences/localPreferencesMeshing.cpp \
           src/gui/preferences/localPreferencesSystem.cpp \
           src/gui/preferences/preferences.cpp \
           src/gui/preferences/preferences2d.cpp \
           src/gui/preferences/preferences3d.cpp \
           src/gui/preferences/preferencesMesh.cpp \
           src/system/events/omPreferenceEvent.cpp \
           src/system/events/omProgressEvent.cpp \
           src/system/events/omSegmentEvent.cpp \
           src/system/events/omSystemModeEvent.cpp \
           src/system/events/omToolModeEvent.cpp \
           src/system/events/omView3dEvent.cpp \
           src/system/events/omViewEvent.cpp \
           src/system/events/omVoxelEvent.cpp \
           src/system/viewGroup/omViewGroupState.cpp \
           src/view3d/widgets/omChunkExtentWidget.cpp \
           src/view3d/widgets/omInfoWidget.cpp \
           src/view3d/widgets/omSelectionWidget.cpp \
           src/view3d/widgets/omViewBoxWidget.cpp \
           src/segment/actions/omSegmentEditor.cpp \
           src/segment/actions/edit/omEditSelectionSetAction.cpp \
           src/segment/actions/segment/omSegmentJoinAction.cpp \
           src/segment/actions/segment/omSegmentSelectAction.cpp \
           src/segment/actions/segment/omSegmentSplitAction.cpp \
           src/segment/actions/voxel/omVoxelSelectionAction.cpp \
           src/segment/actions/voxel/omVoxelSetAction.cpp \
           src/segment/actions/voxel/omVoxelSetConnectedAction.cpp \
           src/segment/actions/voxel/omVoxelSetValueAction.cpp \
           src/system/omThreadedCache.cpp \
           src/system/omFetchingThread.cpp \
           src/system/omGenericManager.cpp \
           src/system/omGroups.cpp \
           src/system/omGroup.cpp \
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
