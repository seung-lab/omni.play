# Requirements Documentation
[TOC]

\section md_requirements The requirements for OMNI
#### An application for visualizing, editing, and skeletonizing images of the brain ####

 1.  Working with single projects will be possible within a single Omni session.
 
 1.1.  A project will be contained within a single .omni file.
 
 1.2.  A project will oriented within a single spacial grid.
 
 1.2.1.  A mini-map of the spatial grid will exist.
 
 1.2.2.  Multiple color tables will be able to exist inside a single project.
 
 1.2.3.  Multiple volumes will be able to exist inside of a single project.
 
 1.2.3.1.  Importing volumes for multiple sources will be possible.
 
 1.2.3.1.1.  Copying a volume from one project to another will be possible.
 
 1.2.3.1.2.  Importing volumes from series of images will be possible.
 
 1.2.3.1.2.1.  Dicing of serial images will result in an “octree” on disk.
 
 1.2.3.1.3.  Importing volumes from a script will be possible.
 
 1.2.3.2.  Multiple volume types will be supported.
 
 1.2.3.2.1.  A raw channel volume type will be supported.
 
 1.2.3.2.1.1.  Raw channel volume types will default to read-only.
 
 1.2.3.2.1.2.  By default raw channel volumes will be loaded from a single stack of gray-scale/raw bitmaps.
 
 1.2.3.2.2.  A segmentation volume type will be supported.
  
 1.2.3.2.2.1.  Individual labels will be registered.
 
 1.2.3.2.2.1.1.  A label will be a whole number.
 
 1.2.3.2.2.2.  By default a segmentation volume will be loaded from a single stack of labeled/segmented bitmaps.
 
 1.2.3.2.3.  A skeleton volume type will be supported.
 
 1.2.3.2.3.1.  A skeleton volume type will be supported.
 
 1.2.3.2.4.  A functional volume type will be supported.
 
 1.2.3.2.4.1.  A functional volume is any volume that implements the  volume interface.
 
 1.2.4.  A project can have many views to visualize  two dimensional or three dimensional representation of the volumes.
 
 1.2.4.1.  Only one view can be the focus of user input any any given time.
 
 1.2.4.2.  Views can be linked.
 
 1.2.4.2.1.  Linked views can be operated on in tandem.
 
 1.2.4.2.1.1.  Lined views will be identifiable by both color and number.
 
 1.2.4.2.1.2.  Zooming will be supported as a tandem operation.
 
 1.2.4.2.1.3.  Viewing will work in two modes.
 
 1.2.4.2.1.3.1.  KNOSSOS [1] compatible browsing will lock the cross-hairs in the center of the views.
 
 1.2.4.2.1.3.1.1.  Panning in one direction in one view will cause linked views to update their position as well.
 
 1.2.4.2.1.3.2.  Native browsing will allow the cross-hairs to move around the views.
 
 1.2.4.2.1.3.2.1.  Panning in one direction in a view will cause the linked views to update their cross-hairs and possibly their position as 
 
 well.
 
 1.2.4.3.  A two dimensional view, will be arranged along two of the three dimensions of the orientation of the volumes, also know as a slice.
 
 1.2.4.3.1.  An colorized two dimensional view can be composed of one or more layers using a combination of applied filters.
 
 1.2.4.3.1.1.  A filter on a layers may change the layer by a percent transparency.
 
 1.2.4.3.1.2.  A filter on a layer may compose a layer from two or more layers using addition.
 
 1.2.4.3.1.3.  A filter on a layer may compose a layer directly from another layer and one other filter operation.
 
 1.2.4.3.1.4. A filter on a layer may apply a false colorization.
 
 1.2.4.3.1.4.1.  A gray-scale to false color layer filter will be supported. 
 
 1.2.4.3.1.5.  A filter on a layer may apply a registered colorization.
 
 1.2.4.3.1.6.  A filter on a layer may effect the visible contrast.
 
 1.2.4.3.1.7.  A filter on a layer may be a projection constructed from one or more slices taken from one or more volumes.
 
 1.2.4.3.1.7.1.  A blend filter will be supported.
 
 1.2.4.3.1.7.2.  A max projection filter will be supported.
 
 1.2.4.3.1.7.3.  A MIP projection filter will be supported.
 
 1.2.4.3.2.  A layer may be edited.
 
 1.2.4.3.2.1.  The pixel values can be changed using a paint brush.
 
 1.2.4.3.2.2.  A segment can be merged with another segment.
 
 1.2.4.3.3.  A layer may be set visible or invisible.
 
 1.2.4.4.  A three dimensional view will visualized by being projected in all three dimensions.
 
 1.2.4.4.1.  Views can be operated on in tandem with the two dimensional views.
 
 1.2.4.4.1.1.  Zooming will be supported as a tandem operation.
 
 1.2.4.4.2.  Raw channel volumes will be represented in three dimensions as three intersecting slices.
 
 1.2.4.4.3.  Segmented volumes will be represented in three dimensions in two ways.
 
 1.2.4.4.3.1.  A voxel representation of the volume as a three dimensional object will be possible.
 
 1.2.4.4.3.2.  As a mesh representation of the volume in three dimensional space will be possible.
 
 1.2.4.4.4. Skeleton volumes will be represented in three dimensional space using shaded trees.
 
 1.2.4.5.  Exporting views will be possible.
 
 1.2.4.5.1.  Exported views as stacks will be able to take the form of a single stack of gray-scale bitmaps.
 
 1.2.4.5.2.  Exported views as stacks will be able to take the form of a single stack of labeled bitmaps.
 
 1.2.4.5.3.  Exported views as stacks will be able to take the form of multiple stacks of color separated gray-scale bitmaps.
 
 1.2.4.5.4.  Exported views as new volumes will be possible.
 
 1.2.5.  A project will have a tool collection.
 
 1.2.5.1.  A pan tool will exist.
 
 1.2.5.2.  A two dimensional erase tool with exist.
 
 1.2.5.3.  A two dimensional paint brush will exist.
 
 1.2.5.3.1.  A toggled smudge mode will exist.
 
 1.2.5.3.2.  A fill mode will exist.
 
 1.2.5.4.  A three dimensional erase tool will exist.
 
 1.2.5.5.  A three dimensional paint brush will exist.
 
 1.2.5.6.  A skeletonizer tool will exist.
 
 1.2.5.7.  A zoom tool with exist.
 
 1.2.5.8.  A voxel/pixel inspection tool will exist.
 
 1.2.5.9.  A segment join tool will exist.
 
 1.2.5.10.  A crosshair tool will exist.
 
 1.2.6.  A project can contain multiple scripts.
 
 1.2.6.1.  Python will be a supported scripting language.
 
 1.2.6.2.  Matlab will be a supported scripting language.
 
 1.2.6.3.  A script can define a tool.
 
 1.2.6.4.  A script can define a filter.
 
 1.2.6.5.  A script can define a volume.
 
 1.2.6.6.  A script can define an exporter.
 
 1.2.6.7.  A script can define an importer.

 \section md_requirements_usecases Use Cases

1. Simple linked view scenarios.
1. Scenarios with two dimensional views linked with three dimensional views.
1. Suppose the user wants to have  a two dimensional view update while panning around in a three dimensional view. The user would create both views, selecting which XY slice was of interest for the two dimensional view. Two link the two views, the user would drag the link button from one view to the link button of the other. Now, both link buttons will be colored the same color to reflect that they are linked. Any tandem operation happening in one view will happen in the other.
2. KNOSSOS compatible skeletonization scenarios.
1. Adding root node scenarios.
2. Adding branch node scenarios.
3. Connecting nodes scenarios.
3.  Browsecomponents compatible viewing scenarios.
1. Multiple segmentations scenarios.
1. In this scenario four two dimensional views are added and link together. In one of the views the raw volume is added to the layer. In the remaining three views three separate volumes containing three different segmentations are added to the views as layers. Because the volume types were set to segmentations for these three volumes, the layers created default to using registered colorization filters.
4. Photoshop-like layers support scenarios.
1. Filtration and combining of multiple layers scenarios.
1. Scenarios for overlaying gray-scale images into a colorized view.
1. In this use case three gray-scale volumes are loaded into a single project. Each of these three volumes are layered into a single view. A false colorization filter is applied to each layer. Each layer is set invisible. A forth layer is added. This forth layer is set visible by default. An additive filter is applied to this forth layer that adds the three false colorized layers into the layer.
2. Editing a layer with brushes scenarios.
1. Choosing brush size shape scenarios.
2. Choosing brush color scenarios.
3. Scenarios for choosing on which layer modifications are made.
4. Smudge like operation scenarios.
1. Toggling smudging scenarios.
1. In this scenario the user decides to toggle paint smudging. Suppose the user has already painted a section of a layer with a color, but the current paint color is different that the previously painted color. The user notices that the previously painted area is missing a bit of color in a section of the layer. By right clicking and selecting “Smudge” from the popup menu the paint tool is now set to smudging. The user now clicks in an area of the image that was painted a color different than the currently selected color, and drags the color from that section to the previously unpainted section. This operation does not change the selected paint color.
2. In this example the user notices that some color has been incorrectly applied at the border of some object. Instead of selecting the erase tool, the user simply right clicks and selects “Smudge” from the popup menu. Then clicking in an unpainted region and drags the lack of color over the area of color that was incorrectly applied.
5. Three dimensional voxel editing scenarios.
6. Combined EM and light microscope scenarios.
1. In this use case the smaller light microscope volume is expanded inline so that it properly overlays the EM volume.
7. Exporting image stack scenarios.
8. Merging an Oversegmentation.
1. The oversegmentation should be loaded and built.
2. Channel data for the over segmentation should be loaded built.
1. A filter should be added to the channel.
1. The filter should point to the first segmentation.
3. View the channel.
4. Double click on a segment in the overlay to select it.
5. Holding down Shift, double click on another segment.
6. Press the “J” key to join the two segments.

 \section md_requirements_especification Especification

 \subsection md_requirements_especification_datastructures Data Structures
 1.  OmId as uint32_t.
 2.  SEGMENT_DATA_TYPE as unsigned int.
 3.  SegmentDataSet as a templated set of SEGMENT_DATA_TYPEs.
 4.  OmCacheableObject as templated interface.
 4.1.  Contains mCache as pointer to an OmGenericCache.
 5. OmGenericManager as templated interface.
 5.1.  Contains mMap as OmId keyed templated map.
 6.  OmGenericCache as templated interface.
 6.1.  Contains mCacheValueMap as templated map.
 7.  OmGenericThreadedCache as templated interface and implementation of OmGenericCache.
 8.  OmManageableObject interface.
 8.1.  Contains mId as OmId.
 8.2. Contains mName as string.
 8.3.  Contains mNote as string.
 8.4.  Contains mIsLocked as int.
 9.  DataBbox as templated vmml::AxisAlignedBoundingBox of int.
 10.  NormBBox as templated vmml::AxisAlignedBoundingBox of float.
 11.  OmSegmentManager interface.
 11.1.  Contains mGenericSegmentManager as OmSegment templated OmGenericManager.
 12.  OmMipMeshCoord.
 12.1. Contains MipChunkCoord as OmMipChunkCoord.
 12.2.  Contains DataValue as SEGMENT_DATA_TYPE.
 13.  OmMipMesh as implementation of templated OmCacableObject with OmMipMeshCoord as key and OmMipMesh as value.
 13.1.  Contains mpMipMeshManager as pointer to OmMipMeshManager.
 14.  OmMipMeshManager as implementation of templated OmGenericThreadedCache with OmMipMeshCoord as key and OmMipMesh as value.
 15.  OmMipChunkMesher.
 15.1.  Contains mspInstance as singleton pointer to OmMipChunkMesher.
 15.2.  Contains mpMipMeshManager as pointer to OmMipMeshManager.
 15.3.  Contains mpCurrentMeshSource as pointer to OmMeshScourse.
 15.4. Contains mCurrentMipCoord as OmMipChunkCoord.
 15.5.  Contains mCurrent SegmentDataSet as SegmentDataSet.
 16.  OmMipVolume as interface.
 16.1.  Contains mBuiltMipChunkCoords as set of OmMipChunkCoord.
 16.2.  Contains mEditiedLeafChunks as set of OmMipChunkCoord.
 17.  DataCoord as templated vmml::Vector3 of int.
 18.  OmMipChunkCoord.
 18.1.  Contains Coordinate as DataCoord.
 19.  OmDataVolume as interface.
 19.1.  Contains mIsOpen as bool.
 19.2.  Contains mIsDirty as bool.
 19.3.  Contains mBytesPerSample as int.
 19.4.  Contains mDataExtent as DataBbox.
 19.5.  Contains mImageData as vtkImageData.
 20. OmMipMeshManager as implementation of OmGenericThreadedCache.
 21.  OmMipChunk as implementation of OmDataVolume and templated OmCacheableObject with OmMipChunkCoord as key and self as value.
 21.1.  Contains mpMipVolume as pointer to parent OmMipVolume.
 21.2.  Contains mCoordinate as OmMipChunkCoord.
 21.3.  Contains mParentCoord as OmMipChunkCoord.
 21.4.  Contains mNormExtent's as NormBBox.
 21.5.  Contains mClippedNormExtents as NormBBox.
 22.  OmMipVolume implementation of templated OmGenericCache with OmMipChunkCoord as key and OmMipChunk as value.
 23.  OmSegmentation as implementation of OmCachingMipVolume and OmManageableObject.
 24.  OmChannel as implementation of OmCachingMipVolume and OmManageableObject.
 25.  OmVolume  and implementation of OmManageableObject.
 25.1.  Contains mspInstance as singleton pointer to OmVolume.
 25.2.  Contains mChannelManager as an OmChannel templated OmGenericManager.
 25.3.  Contains mSegmenationManager as an OmSegmenation templated OmGenericManager.
 26.  OmFilter as implementation of OmManageableObject.
 26.1.  Contains mAlpha as double.
 27.  OmGarbage as sington.
 27.1.  Contains mTextures as a vector of Gluints.

 \subsection md_requirements_especification_algorithms Algorithms
 1. Meshing
 1.1.  Marching Cubes.
 1.1.1.  Implemented in VTK.

 \subsection md_requirements_especification_pythonPython Scripting
 1.  Importing the Omni module.
 1.1.  “import Omni”
 2.  Specifying an Omni filter.
 3.  Specifying an Omni volume.
 4.  Specifying an Omni importer.
 5.  Specifying an Omni exporter.
 6.  Specifying an Omni tool.

 \subsection md_requirements_especification_matlab Matlab Scripting

 \subsection md_requirements_especification_operational (O)perational Profile
 1.  Quit Omni.
 1.1.  File → Quit.
 1.2.  Control + q.
 2.  Create New Project.
 2.1.  File → New.
 2.2.  Control + n.
 3.  Open Project.
 3.1.  File → Open.
 3.2.  Control + o.
 4.  Save Project.
 4.1.  File → Save.
 4.2. Control + s.
 4.3.  Save as new.
 4.4. Overwrite old.
 5.  Close Project.
 5.1.  File → Close.
 5.2. Control + e.
 5.3.  With save.
 5.4.  Without save.
 6.  Undo as a meta-operation.
 6.1.  Edit → Undo.
 6.2.  Control + u.
 7.  Redo as a meta-operation.
 7.1.  Edit → Redo.
 7.2.  Control + r.
 8.  Import volume. 1
 8.1.  File → Import.
 8.2.  Control + i.
 8.3.  Import as channel volume. 2
 8.4.  Import as labeled segmentation volume. 2
 8.5.  Import from script. 2
 9.  Copy volume. 1
 9.1.  Edit → Copy.
 9.2.  Control + c.
 10.  Paste volume. 4
 10.1.  Edit → Paste.
 10.2.  Control + v.
 11.  Export view. 3
 11.1.  File → Export.
 11.2.  Control + t.
 11.3.  Export view as raw bitmaps. 3
 11.4.  Export view as labeled bitmaps. 3
 11.5.  Export view as volume. 4
 12.  Set painting color.
 13.  Set paint brush shape.
 14.  Set paint brush size.
 15.  Toggle paint brush smudging mode.
 16.  Select tool.
 16.1.  Select paint brush tool.
 16.2.  Select zoom tool.
 16.3.  Select pan tool.
 16.4.  Select inspection tool.
 16.5.  Select skeletonizer tool.
 17.  Instantiate view.
 17.1. Link view with existing view.
 17.2.  Instantiate two dimensional view.
 17.2.1.  Set browse mode for view.
 17.2.1.1.  Set classic browse mode.
 17.2.1.2.  Set KNOSSOS Compatible browse mode.
 17.2.2.  Add layer.
 17.2.2.1.  Set layer source volume.2
 17.2.2.2.  Set layer visibility.
 17.2.2.3.  Set layer alpha color.
 17.2.2.4.  Instantiate filter on layer.
 17.2.2.4.1.  Set registered colorization filter.
 17.2.2.4.2.  Set transparency filter.
 17.2.2.4.3.  Set false colorization filter.
 17.2.2.4.4.  Set additive filter.
 17.2.2.5.  Remove filter from layer.
 17.2.2.6.  Set layer as paint target.
 17.2.2.6.1.  Paint on layer.
 17.2.2.7.  Set classic Omni browsing mode.
 17.2.2.8.  Set KNOSSOS compatible browsing mode.
 17.2.3.  Set skeletonization mode.
 17.2.3.1.  Add node.
 17.2.3.2.  Make node a branch node.
 17.2.3.3.  Connect node.
 17.3.  Instantiate three dimensional view.
 17.3.1 . View volume.
 17.3.1.1.  View as mesh.
 17.3.1.2. View as voxels.
 17.3.1.3. View as skeleton.
 17.3.1.4.  Rotate volume.
 17.4. Edit volume.
 17.4.1.  Add voxel.
 17.4.2.  Remove voxel.
 17.4.3.  Change voxel label.
 17.4.4.  Label component.
 17.5.  Zoom in.
 17.5.1.  Equals key.
 17.5.2.  Mousewheel up.
 17.6.  Zoom out.
 17.6.1.  Minus key.
 17.6.2.  Mousewheel down.
 17.7.  Pan camera in.
 17.7.1.  W key.
 17.8.  Pan camera out.
 17.8.1.  S Key.
 17.9.  Pan camera right.
 17.9.1.  D Key.
 17.9.2.  Arrow right key.
 17.9.3.  Click + drag mouse left.
 17.10.  Pan camera left.
 17.10.1.  A key.
 17.10.2.  Arrow left key.
 17.10.3.  Click + drag mouse right.
 17.11.  Pan camera up.
 17.11.1.  E key.
 17.11.2.  Q key.
 17.11.3.  Arrow up key.
 17.11.4.  Click + drag mouse down.
 17.12.  Pan camera down.
 17.12.1.  Z key.
 17.12.2.  X key.
 17.12.3.  Arrow down key.
 17.12.4.  Click + drag mouse up.
 17.13.  Set voxel/pixel selection tool.
 17.13.1.  Inspect pixel.
 17.13.2.  Inspect voxel.
 17.14.  Create script.
 17.15.  Set script editor.
 17.16.  Edit Script.
 17.17.  Save script.
 17.17.1.  Save new script.
 17.17.2.  Overwrite script.
 17.18.  Debug script.
 17.18.1.  Start script.
 17.18.2.  End script.
 17.19.  Register script.

 \subsection md_requirements_especification_functional (F)unctional Profile
 1.  Omni Quit Functionality
 2.  Project Close Functionality
 3.  Project Save Functionality
 4.  Project New Functionality
 5.  Project Open Functionality
 6.  Volume Copy Functionalities
 6.1.  Volume Copy Start Functionality
 6.2.  Volume Copy End Functionality
 7.  Volume Importer Functionalities 1
 7.1.  Volume Importer New Functionality
 7.2.  Volume Importer Build Functionality
 7.2.1.  Volume Importer Build Safe Functionality 
 8.  Volume Exporter Functionalities 3
 8.1.  Volume Exporter New Functionality
 8.2.  Volume Exporter Export Functionality
 9.  Tool Functionalities
 9.1.  Tool New Functionality
 9.2.  Tool Mouse Functionalities
 9.3.  Tool Keyboard Functionalities
 9.4.  Tool Undo Functionality
 9.5.  Tool Redo Functionality
 10. View Functionalities
 10.1.  Instantiate Two Dimensional View Functionality
 10.2.  Instantiate Three Dimensional View Functionality
 10.3.  View Draw Functionalities
 10.3.1.  View Draw Three Dimensional Functionalities
 10.3.1.1.  View Draw Three Dimensional Mesh Functionalities 3
 10.3.1.2.  View Draw Three Dimensional Voxels Functionalities 3
 10.3.2.  View Draw Two Dimensional Functionalities
 11.  Link View Functionalities
 11.1.  Link View Drag Functionality
 11.2.  Link View Drop Functionality
 11.3.  Link View Undo Functionality
 11.4.  Link View Redo Functionality
 12.  Pan Functionalities
 12.1.  Pan Undo Functionality
 12.2.  Pan Redo Functionality
 13.  Zoom Functionalities
 13.1.  Zoom Undo Functionality
 13.2.  Zoom Redo Functionality
 14.  Scripting Functionalities
 14.1.  Script New Functionality
 14.2.  Script Save Functionality
 14.3.  Script Open Functionality
 14.4.  Script Register Functionality
 14.5.  Script Unregister Functionality
 14.6.  Script Run Functionalities
 14.6.1.  Script Run Debug Functionalities
 14.6.1.1.  Script Run Debug Start Functionality
 14.6.1.2.  Script Run Debug End Functionality
 14.6.2.  Script Run Registered Functionalities
 14.6.2.1.  Script Run Registered On Load
 14.6.2.2.  Script Run Registered On Registration
 15.  Layer Functionalities
 15.1.  Layer New Functionality
 15.2.  Layer Set Transparency Functionality
 16.  Filter Functionalities
 16.1.  Filter New Functionality
 16.2.  Filter Filter Functionality

 \subsection md_requirements_especification_modular (M)odular Profile
  
\section md_requirements_authors Authors
 1.  Brett Warne
 1.1.  brett.warne@gmail.com
 1.2.  bwarne@mit.edu
 2.  Rachel Welles Shearer
 3.  Matt Wimer
 3.1.  subsilico@yahoo.com
 3.2.  mwimer@mit.edu

 \section md_requirements_document Document Versions
Current Version
V0.3 – September 17th, 2009
Previous Versions
V0.2 – September 15th, 2009
V0.1 – September 11th, 2009
V0.0 – September 8th, 2009

\section md_requirements_appendices Appendices

\subsection md_requirements_appendixA Appendix A – OxFxM

O. View2d viewing.
	F. Garbage collecting the OpenGL texture ids.
		M. OmGarbage::Instance(). 
1. Creates the singleton in memory if it doesn't exist.
2. Initializes the mutex lock.
3. Returns the OmGarbage pointer.
1. M. OmGarbage::Delete().
1. Destroys the mutex lock.
2. Deletes the OmGarbage singleton.
		M. OmGarbage::asOmTextureId().
1. Locks the mutex.
2. Adds the texture to the list of textures needing freed.
3. Unlocks the mutex.
		M. OmGarbage::Lock().
			1. Locks the mutex.
		M. OmGarbage::LockTextures().
1. Locks the mutex.
2. Returns a reference to the textures needing to be removed.
		M. OmGarbage::Unlock().
			1. Unlocks the mutex.
		M. OmUnlockTextures().
1. Unlocks the mutex.
		M. OmGarbage::safeCleanTexturesIds().
1. Locks the mutex.
2. Calls glDeleteTextures on the list of textures.
3. Clears the list of textures.
	F. Filtering based on OmMipVolume.
		M. OmFilter:GetCache ().
1. Calls OmCachingThreadedCachingTile().
2. Returns the new fast cache for the segmentation if it exists or the channel.
		M. OmFilter::SetSegmentation().
1. Calls OmVolume::GetSegmentation().
1. M. OmFilter::SetChannel().
1. Calls OmVolume::GetChannel().
		M. OmFilter::GetAlpha ().
1. Returns private mAlpha as a double.
1. M. OmFilter::SetAlpha().
			1. Sets mAlpha.

O. Unknown.
	F. Unknown.
		M. OmVolume::DataToNormBbox().
1. Calls OmVolume::DataToNormCoord();
2. Calls NormBbox().
3. Returns the normalized bounding box.
O. Unknown.
	F. Unknown.
		M. OmMipVolume::MipCoordToNormBbox ().
1. Calls OmMipVolume::MipCoordToDataBbox().
2. Calls OmVolume::DataToNormBbox().
3. Returns the normalized bounding box.
O. Unknown.
	F. Unknown.
		M. OmMipVolume::MipCoordToDataBbox ().
1. Calls OmMipVolume::GetChunkDimensions().
2. Calls DataBbox().
3. Returns the extent of the data in the leaf volume that the OmMipCoord covers.
O. Unknown.
	F. Unknown.
		M.  OmMipVolume::GetChunkDimensions().
1. Calls OmMipVolume::GetChunkDimension().
2. Returns the chunk dimensions of the volume as a templated Vector3 of int.
O. Unknown.
	F. Unknown.
		M.  OmMipVolume::GetChunkDimension().
3. Calls OmVolume::GetChunkDimension().
4. Returns the chunk dimension of the volume.
O. Unknown.
	F. Unknown.
		M. OmVolume::GetChunkDimension().
1. Calls Instance().
2. Returns the instance's mChunkDim.
O. Unknown.
	F. Unknown.
		M. OmVolume::NormToDataBbox ().
1. Calls OmVolume::GetDataExtent().
2. Calls DataBbox::getMin().
3. Calls DataBbox::getMax().
4. Calls OmVolume::NormToDataCoord().
5. Returns the DataBbox returned from a call to OmDataVolume::NormToDataCoord() with the min and the max normalized by the pixel dimensions.
O. Unknown.
	F. Unknown.
		M. OmVolume::NormToDataCoord().
1. Calls OmVolume::GetDataExtent ().
2. Calls DataBbox::getMin().
3. Calls DataBbox::getMax().
4. Calls floor().
5. Calls DataCoord().
6. Returns the floor of the normalized coordinates times the scale of each axis as a DataCoord.
O. Unknown.
	F. Unknown.
		M. OmVolume::GetDataExtent().
1. Returns member mDataExtent.
O. Unknown.
	F. Unknown.
		M. OmMipChunk::InitChunk().
1. Calls OmMipVolume::GetRootMipLevel().
2. Calls OmMipVolume::ParentCoord().
3. Calls OmMipVolume::ValidMipChunkChildren().
4. Calls OmMipVolume::MipCoordToDataBox().
1. Sets mDataExtent to the extents of the data.
5. Calls  OmMipVolume::MipCoordToNormBbox().
1. Sets mNormExtents to the normalized extents.
6. Calls NormBbox::intersect().
7. Calls OmDataVolume::OmMipChunkSetBytesPerSample().
8. Calls OmMipChunkCoord::ParentCoord().

\subsection md_requirements_appendixB Appendix B – Figures
\subsection md_requirements_appendixC Appendix C – Example Scripts
\subsection md_requirements_appendixD Appendix D – Dependencies
 1.  VTK – 5.4.2
 2.  QT – 4.5.2
 3.  libtiff
 4.  libpng – 1.2.39
 5.  Python
 6.  Boost – 1_38_0
 7.  HDF5 – 1.6.9
 8.  FontConfig – 2.7.1
 9.  FreeType – 2.3.9
 10.  OpenMPI
 11.  CMake – 2.6
 12.  expat – 2.0.1
 13.  Matlab
\subsection md_requirements_appendixE Appendix E – Test Cases
\subsection md_requirements_appendixF Appendix F – Definitions
 1.  Channel
 2.  Segmentation
 3.  Filter
\subsection md_requirements_appendixG Appendix G – Limitations
 1.  MipChunkCoords that have around a 23 level causes the members of mNormExtent to be negative. 7

\section md_requirements_references References
1. Knossos Specification v1.2. Unknown author.
2. A System for Scalable 3D Visualization and Editing of Connectomic Data , Brett Warne, 2009
3. Omni: Visualizing and Editing Large-Scale Volume Segmentations of Neuronal Tissue , Rachel Welles Shearer, 2009