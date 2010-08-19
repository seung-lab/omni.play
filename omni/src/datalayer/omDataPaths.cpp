#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "mesh/omMipMeshCoord.h"
#include "mesh/omMipMeshManager.h"
#include "project/omProject.h"
#include "system/omLocalPreferences.h"
#include "system/omStateManager.h"
#include "volume/omChannel.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omSegmentation.h"

string OmDataPaths::getDefaultHDF5channelDatasetName()
{
	return "chanSingle";
}

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

string OmDataPaths::getLocalPathForHd5fChunk(OmMipMeshCoord & meshCoordinate, const OmId segmentationID)
{
  QString p = QString("%1.%2.%3_%4_%5.%6.%7.h5")
    .arg(segmentationID)
    .arg(meshCoordinate.MipChunkCoord.Level)
    .arg(meshCoordinate.MipChunkCoord.Coordinate.x)
    .arg(meshCoordinate.MipChunkCoord.Coordinate.y)
    .arg(meshCoordinate.MipChunkCoord.Coordinate.z)
    .arg( OmProject::GetFileName() )
    .arg( OmStateManager::getPID() );

  QString ret = OmLocalPreferences::getScratchPath() + "/meshinator_" + p;
  debug("parallel", "parallel mesh fs path: %s\n", qPrintable( ret ) );
  fprintf(stderr, "parallel mesh fs path: %s\n", qPrintable( ret ) );
  return ret.toStdString();
}

std::string OmDataPaths::getDirectoryPath(OmSegmentation* seg)
{
	return str( boost::format("segmentations/%1%/")
		    % seg->GetId());
}

std::string OmDataPaths::getDirectoryPath(OmChannel* chan)
{
	return str( boost::format("channels/%1%/")
		    % chan->GetId());
}

std::string OmDataPaths::MipLevelInternalDataPath(const std::string & dirPath,
						  const int level)
{
	return dirPath
		+ boost::lexical_cast<std::string>(level)
		+ "/"
		+ "volume.dat";
}

std::string OmDataPaths::MipChunkMetaDataPath(const std::string & dirPath,
					      const OmMipChunkCoord & coord)
{
	QString p = QString("%1/%2_%3_%4/")
		.arg(coord.Level)
		.arg(coord.Coordinate.x)
		.arg(coord.Coordinate.y)
		.arg(coord.Coordinate.z);

	return dirPath + p.toStdString() + "metachunk.dat";
}
