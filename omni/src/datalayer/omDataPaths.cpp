#include "common/omCommon.h"
#include "common/omDebug.h"
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

std::string OmDataPaths::getDefaultHDF5channelDatasetName()
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

OmDataPath OmDataPaths::getSegmentPagePath(const OmID segmentationID,
										   const quint32 pageNum)
{
	const std::string p =
		str( boost::format("segmentations/segmentation%1%/segment_page%2%")
			 % segmentationID
			 % pageNum );

	return OmDataPath(p);
}

std::string OmDataPaths::getMeshDirectoryPath(const OmMipMeshCoord& meshCoord,
											  OmMipMeshManager* const mipMeshManager)
{
	const std::string p = str( boost::format("%1%/%2%_%3%_%4%/mesh/%5%/")
							   % meshCoord.MipChunkCoord.Level
							   % meshCoord.MipChunkCoord.Coordinate.x
							   % meshCoord.MipChunkCoord.Coordinate.y
							   % meshCoord.MipChunkCoord.Coordinate.z
							   % meshCoord.DataValue);

	return mipMeshManager->GetDirectoryPath() + p;
}

std::string OmDataPaths::getMeshFileName( const OmMipMeshCoord & meshCoord )
{
	return str( boost::format("mesh.%1%.dat")
				%meshCoord.DataValue);
}

std::string OmDataPaths::getLocalPathForHd5fChunk(const OmMipMeshCoord& meshCoord,
											 const OmID segmentationID)
{
	const QString p = QString("%1.%2.%3_%4_%5.%6.%7.h5")
		.arg(segmentationID)
		.arg(meshCoord.MipChunkCoord.Level)
		.arg(meshCoord.MipChunkCoord.Coordinate.x)
		.arg(meshCoord.MipChunkCoord.Coordinate.y)
		.arg(meshCoord.MipChunkCoord.Coordinate.z)
		.arg( OmProject::GetFileName() )
		.arg( OmStateManager::getPID() );

	const QString ret =
		OmLocalPreferences::getScratchPath() + "/meshinator_" + p;

	//debug(parallel, "parallel mesh fs path: %s\n", qPrintable( ret ) );
	fprintf(stderr, "parallel mesh fs path: %s\n", qPrintable( ret ) );

	return ret.toStdString();
}

std::string OmDataPaths::getDirectoryPath(OmSegmentation* seg)
{
	return str( boost::format("segmentations/segmentation%1%/")
				% seg->GetID());
}

std::string OmDataPaths::getDirectoryPath(OmChannel* chan)
{
	return str( boost::format("channels/channel%1%/")
				% chan->GetID());
}

std::string OmDataPaths::MipLevelInternalDataPath(const std::string & dirPath,
												  const int level)
{
	return dirPath
		+ om::NumToStr(level)
		+ "/"
		+ "volume.dat";
}

std::string OmDataPaths::MipChunkMetaDataPath(const std::string & dirPath,
											  const OmMipChunkCoord & coord)
{
	const QString p = QString("%1/%2_%3_%4/")
		.arg(coord.Level)
		.arg(coord.Coordinate.x)
		.arg(coord.Coordinate.y)
		.arg(coord.Coordinate.z);

	return dirPath + p.toStdString() + "metachunk.dat";
}
