#include "volume/omMipChunk.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataReader.h"
#include "project/omProject.h"
#include "system/omBuildChannel.h"
#include "system/omProjectData.h"
#include "utility/omImageDataIo.h"
#include "utility/stringHelpers.h"

#include <vtkImageData.h>

#include <QTextStream>
#include <QImage>

OmBuildChannel::OmBuildChannel(OmChannel * chan)
	: OmBuildVolumes()
{
	mChann = chan;
}

void OmBuildChannel::build_channel()
{
	start();
}

void OmBuildChannel::run()
{
	const QString type = "channel";
	if( !checkSettings() ){
		return;
	}

	startTiming(type);

        if( 0 ) {
		newBuild();
	} else {
		mChann->SetSourceFilenamesAndPaths( mFileNamesAndPaths );
		mChann->BuildVolumeData();
	}

	stopTiming(type);
}

void OmBuildChannel::newBuild()
{
	//////////////////
	//////////////////
	// check source data
	QImage image( mFileNamesAndPaths.at(0).filePath() );
	const int x = image.width();
	const int y = image.height();
	const int z = mFileNamesAndPaths.size();
	mDepth = image.depth();

	mChann->SetBuildState(MIPVOL_BUILDING);

	//////////////////
	//////////////////
	// MIP stuff
	Vector3 < int > image_dims( x, y, z );
	if (mChann->GetDataDimensions() != image_dims) {
		printf("OmMipVolume::UpdateMipProperties: CHANGING VOLUME DIMENSIONS\n");
		mChann->SetDataDimensions(image_dims);
	}
	if (mChann->GetChunkDimension() % 2) {
		throw OmFormatException("Chunk dimensions must be even.");
	}
	// UpdateRootLevel
	Vector3 < int >source_dims = mChann->GetDataExtent().getUnitDimensions();
	int max_source_dim = source_dims.getMaxComponent();
	int mipchunk_dim = mChann->GetChunkDimension();

	if (max_source_dim <= mipchunk_dim) {
		mChann->mMipRootLevel = 0;
	} else {
		//use log base 2 to determine levels needed to contain source dims
		mChann->mMipRootLevel = ceil(log((float) (max_source_dim) / mChann->GetChunkDimension()) / log((float)2));
	}

	//////////////////
	//////////////////
	// nuke old data
	OmDataPath volPath;
	volPath.setPathQstr( QString("channels/%1/").arg(mChann->GetName()) );
	if (OmProjectData::GetProjectDataReader()->group_exists(volPath)) {
		OmProjectData::GetDataWriter()->group_delete(volPath);
	}

	//////////////////
	//////////////////
	// AllocInternalData
	const int chunkSize = mChann->GetChunkDimension();
	Vector3<int> chunkdims = Vector3 < int >( chunkSize, chunkSize, chunkSize );
	for (int i = 0; i <= mChann->GetRootMipLevel(); i++) {
		Vector3 < int >data_dims = MipLevelDataDimensions(i);
		//round up to nearest chunk
		Vector3 < int >rounded_data_dims = Vector3 < int >(ROUNDUP(data_dims.x, chunkSize),
								   ROUNDUP(data_dims.y, chunkSize),
								   ROUNDUP(data_dims.z, chunkSize));
		
		OmDataPath mip_volume_level_path;
		QString mip_path = QString("%1%2/%3")
			.arg( QString("channels/%1/").arg(mChann->GetName() ) )
			.arg(i)
			.arg( QString::fromStdString( "volume.dat" ));		
		mip_volume_level_path.setPathQstr( mip_path );
		
		//debug("genone","OmMipVolume::AllocInternalData: %s \n", mip_volume_level_path.data());
		OmProjectData::GetDataWriter()->dataset_image_create_tree_overwrite( mip_volume_level_path, 
										     &rounded_data_dims,
										     &chunkdims, 
										     mDepth);
	}

	//////////////////
	//////////////////
	// ImportSourceData
	foreach( QFileInfo finfo, mFileNamesAndPaths ){
		QImage image( finfo.filePath() );
		// TODO: save into hdf5 hyperslab; hopefully just pass image.bits() straight in
		//  (QImage does a deep copy of the data that is returns w/ bits())
	}


	//////////////////
	//////////////////
	// BuildVolume
	BuildVolume();

	// if everything worked...
	mChann->SetBuildState(MIPVOL_BUILT);
}


bool OmBuildChannel::BuildVolume()
{
	//for each level
	for (int level = 0; level <= mChann->GetRootMipLevel(); ++level) {

		//dim of miplevel in mipchunks
		Vector3 < int >mip_coord_dims = mChann->MipLevelDimensionsInMipChunks(level);

		//for all coords
		for (int z = 0; z < mip_coord_dims.z; ++z) {
			for (int y = 0; y < mip_coord_dims.y; ++y) {
				for (int x = 0; x < mip_coord_dims.x; ++x) {
					BuildChunk(OmMipChunkCoord(level, x, y, z));
				}
			}
		}
		mChann->Flush();
	}

	return true;
}

void OmBuildChannel::BuildChunk(const OmMipChunkCoord & rMipCoord)
{
	//debug("genone","OmMipVolume::BuildChunk()\n");

	//leaf chunks are skipped since no children to build from
	if (rMipCoord.IsLeaf())
		return;

	//otherwise chunk is a parent, so get pointer to chunk
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	mChann->GetChunk(p_chunk, rMipCoord);

	//read original data
	OmDataPath source_data_path;
	QString mip_path = QString("%1%2/%3")
		.arg( QString("channels/%1/").arg(mChann->GetName() ) )
		.arg( rMipCoord.Level - 1)
		.arg( QString::fromStdString( "volume.dat" ));
	source_data_path.setPathQstr( mip_path );
	DataBbox source_data_bbox = MipCoordToDataBbox(rMipCoord, rMipCoord.Level - 1);

	//read and get pointer to data
	vtkImageData *p_source_data =
		OmProjectData::GetProjectDataReader()->dataset_image_read_trim(source_data_path, 
									source_data_bbox, 
									mDepth );

	//subsample
	vtkImageData *p_subsampled_data = NULL;

#if 0  // oh dear, not fun looking
	
	//switch on scalar type
	switch ( mDepth ) {
	case 1:
		p_subsampled_data = SubsampleImageData < unsigned char >(p_source_data, GetSubsampleMode());
		break;
	case 4:
		p_subsampled_data = SubsampleImageData < unsigned int >(p_source_data, GetSubsampleMode());
		break;
	default:
		assert(false);
	}
#endif 

	//set or replace image data (chunk now owns pointer)
	p_chunk->SetImageData(p_subsampled_data);

	//delete source data
	p_source_data->Delete();
	p_source_data = NULL;
}

/*
 *	Calculate the data dimensions needed to contain the volume at a given compression level.
 */
Vector3 < int > OmBuildChannel::MipLevelDataDimensions(int level)
{
	//get dimensions
	DataBbox source_extent = mChann->GetDataExtent();
	Vector3 < float >source_dims = source_extent.getUnitDimensions();

	//dims in fraction of pixels
	Vector3 < float >mip_level_dims = source_dims / OMPOW(2, level);

	return Vector3 < int >(ceil(mip_level_dims.x), ceil(mip_level_dims.y), ceil(mip_level_dims.z));
}

/*
 *	Returns the extent of the data in specified level covered by the given MipCoordinate.
 */
DataBbox OmBuildChannel::MipCoordToDataBbox(const OmMipChunkCoord & rMipCoord, int newLevel)
{

	int old_level_factor = OMPOW(2, rMipCoord.Level);
	int new_level_factor = OMPOW(2, newLevel);

	const int chunkSize = mChann->GetChunkDimension();
	Vector3<int> chunkdims = Vector3 < int >( chunkSize, chunkSize, chunkSize );

	//convert to leaf level dimensions
	int leaf_dim = chunkSize * old_level_factor;
	Vector3 < int >leaf_dims = chunkdims * old_level_factor;

	//min of extent in leaf data coordinates
	DataCoord leaf_min_coord = rMipCoord.Coordinate * leaf_dim;

	//convert to new level
	DataCoord new_extent_min_coord = leaf_min_coord / new_level_factor;
	Vector3 < int >new_dims = leaf_dims / new_level_factor;

	//return
	return DataBbox(new_extent_min_coord, new_dims.x, new_dims.y, new_dims.z);
}
