# Architecture/Design documentation
[TOC]

\section md_architecture  OMNI


What is the file structure?
the chunck folder contains a tree structure folders.
the first level corresponds to the mip level while, the other 3 levels corresponds to x/y/z coordinates.

\subsection md_architecure_view3d

It's recomended to read bwarne-meng-2009.pdf

A chunk is a cube which contains a certain amount of voxel, with either channel or segmentation values. The amount of voxel is in correspondences with the mip level (which is a number between 1 and 4) that describes the ammount of detail. If is level 1 then it has 128 ^ 3 voxels. 
1 128^3
2 64^3
3 32^3
4 16^3
5 8^3


A segment is a volume with a irregular shape , which is contained within a number of chunks, but not necessary covers the hole volume of all of the chunks.
Segment. While the shape of a 3D segment object is defined by data in the segmentation volume, the segment component serves to manage the abstract representations of the data. This abstraction allows the segmentation data to be associated in more complex ways, allowing for greater flexibility and performance during visualization and editing.

A mesh plan , inherits a std::multimap which is an associative container that store elements formed by a combination of a key value and a mapped value, following a specific order, and where multiple elements can have equivalent keys. In this case, the key is the segment , and the mapped value are the chunks, and the bigger segments are stored first.


The system makes use of both voxel and mesh rendering depending on the intentions of the user to take advantage of the benefits of each representation.
During general visualization, the system displays all data using meshes to quickly load and display 3D segment objects. When a specific segment has been specified to be edited, the system takes the time to generate and render a voxel representation to make use of the fine editing granularity of voxels. Unlike meshes that are build during pre-processing, voxel representations are constructed as need, due to the large memory requirements of storing such
a detailed representation for all segment objects. The fast performance of meshes for gen-eral 3D visualization and the detail of voxels for editing optimizes the system functionality depending on visualization or editing intentions of the user.

Volume. The volume component manages the segmentation representation. It serves as the interface for the other components to access, manipulate, and render segmentation volume data. The textitvolume component depends on the mesh component to render mesh versions segment data, the segment component to for access to segment data properties, and the system component to perform updates.Volume class enables access to all data managed by the system. A Volume contains no direct access to data, but manages references to multiple sets of segmentation data. In addition to managing access, a Volume also performs the conversion between the multiple coordinate frames the system uses

MipVolume: To achieve the scalable performance, the data of a segmentation is abstracted further into a MipVolume. This class applies the mip-mapping technique of generating pyramidal data structures in an oct-tree hierarchy to efficiently compress multiple scaled versions of the data

MipChunk: A MipChunk augments the compression of a MipVolume with a hierarchical coordinate system so as to allow a MipVolume to define and manage regions of data within mip-level volumes. A MipChunk contains directly accessible data addressed as cube of uniform dimensions containing data extracted from a mip-level volume.The parent MipChunk and octal children MipChunks share the same extent in space, although the parent is a subsampled version of the region covered by the children. The MipChunk that entirely contains the root mip-level volume is known as the root MipChunk and does not have a parent.
Each MipChunk summarizes relevant information into associated metadata. The most significant portions of this metadata are sets of data values that uniquely correspond to segmented regions in the volume data.

A mesh is a triangle strip which represent the frotier of a given segment.Mesh. A mesh representation of a 3D object offers considerable performance bene-
fits over a voxel representation. As a collection of triangles that forms a surface, a mesh naturally avoids time wasted on rendering the internal regions of a 3D object. Furthermore, unlike the regular structure of voxels, the set of triangles that form a mesh can be irregularly sized. This means that large uniform portions of the surface area of a mesh can be approximated by a few large triangles which load and render quickly, while detailed portions can be preserved by many smaller triangles. The level of detail preserved is a parameter of the mesh generation process, and allows for user defined selection of performance or detail preservation. The mesh component of the system manages the construction and rendering of the mesh representation of 3D segment objects. It uses the volume component to extract data to reconstruct into a surface and the segment component for access to the segment data properties needed for rendering.

MipMesh: A MipMesh encapsulates the data structure and functions needed to draw a tessellated surface representation of a portion of a 3D segment object to the screen. Specifically, a MipMesh is a reconstructed surface mesh defining a region of similar values contained within the volume data of a single MipChunk.A traditional approach to 3D object meshing would associate each mesh to a single unique segment, regardless of the boundaries of MipChunks. In this way, only a single mesh object would need to be rendered to represent all the data of a single value. In contrast, the method of breaking a single mesh into pieces, or MipChunks, as proposed by this system offers significant scalability benefits.

when multiple MipMeshes are used to describe a single surface, they must contain redundant border geometry along their seams so as to appear to be a uniform surface. Due to the independent nature of each MipMesh, this boundary geometry data is impractical to share and so adjacent MipMeshes must contain
redundantly copies of this data.

System. The system component facilitates communication and state managing to ensure synchronicity between components.

View. The view component controls the user interface and rendering calls during visualization and editing. The view uses the structure of the volume component to efficiently render segmentations and the system component to synchronize screen redraws so as to reflect changes that are relevant to the 3D visualization.

\subsubsection culling

Utilizing the oct-tree structure of MipChunks, the system uses hierarchical rendering tech-
niques to efficiently limit processing to relevant segmentation data in a MipVolume during
the visualization process.