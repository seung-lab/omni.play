#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "mesh/omMipMeshCoord.h"
#include "mesh/omMipMeshManager.h"

OmDataPath OmDataPaths::getDefaultDatasetName()
{
	return OmDataPath("main");
}

OmDataPath OmDataPaths::getProjectArchiveNameQT()
{
	return OmDataPath("project.qt.dat");
}

OmDataPath OmDataPaths::getSegmentPagePath( const OmId segmentationID, const quint32 pageNum )
{
	string p = str( boost::format("segmentations/segmentation%1%/segment_page%2%") 
			% segmentationID
			% pageNum );
	
	return OmDataPath(p);
}

string OmDataPaths::getMeshDirectoryPath( const OmMipMeshCoord & meshCoordinate,
					  OmMipMeshManager * const mipMeshManager )
{
	string p = str( boost::format("%1%/%2%_%3%_%4%/mesh/%5%/")
			% meshCoordinate.MipChunkCoord.Level
			% meshCoordinate.MipChunkCoord.Coordinate.x
			% meshCoordinate.MipChunkCoord.Coordinate.y
			% meshCoordinate.MipChunkCoord.Coordinate.z
			% meshCoordinate.DataValue);
			
			
	return mipMeshManager->GetDirectoryPath().toStdString() + p;
}

string OmDataPaths::getMeshFileName( const OmMipMeshCoord & meshCoordinate )
{
	return str( boost::format("mesh.%1%.dat")
		    %meshCoordinate.DataValue);
}
