#ifndef OM_VOLUME_BUILDER_HPP
#define OM_VOLUME_BUILDER_HPP

#include "volume/build/omVolumeImporter.hpp"
#include "volume/omVolumeData.hpp"
#include "utility/sortHelpers.h"
#include "common/omDebug.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omIDataReader.h"
#include "system/omProjectData.h"
#include "utility/omTimer.h"
#include "volume/omMipVolume.h"

#include <QFileInfo>
#include <QImage>

template <typename VOL>
class OmVolumeBuilder {
private:
	VOL *const vol_;
	om::Affinity aff_;

	std::vector<QFileInfo> files_;
	bool sourceFilesWereSet;

public:
	OmVolumeBuilder(VOL* vol, const om::Affinity aff=om::NO_AFFINITY)
		: vol_(vol)
		, aff_(aff)
		, sourceFilesWereSet(false)
	{
		printf("2 aff_ %i\n", aff_);
	}

	void SetSourceFilenamesAndPaths(const std::vector<QFileInfo>& srcFiles)
	{
		files_ = srcFiles;
		SortHelpers::SortNaturally(files_);
		sourceFilesWereSet = true;
	}

	void DeleteVolumeData()
	{
		OmDataPath path(vol_->GetDirectoryPath());

		OmProjectData::DeleteInternalData(path);
	}

	void Build(OmDataPath & dataset)
	{
		//if source data valid
		if(!isSourceValid()) {
			throw OmIoException("source files not found");
		}

		//unbuild
		vol_->SetBuildState(MIPVOL_BUILDING);

		//update properties
		UpdateMipProperties(dataset);

		//delete old
		DeleteVolumeData();

		//copy source data
		if (!ImportSourceData(dataset)) {
			throw OmIoException("could not import source files");
		}

		vol_->getVolData()->downsample(vol_);

		//build volume
		if (!BuildThreadedVolume()) {
			throw OmIoException("volume build failed");
		}

		//build complete
		vol_->SetBuildState(MIPVOL_BUILT);
	}

private:

	bool ImportSourceData(const OmDataPath& path)
	{
		OmVolumeImporter<VOL> importer(vol_, path,
									   areImportFilesImages(),
									   files_, aff_);
		return importer.Import();
	}

	bool BuildThreadedVolume()
	{
		OmTimer timer;

		try{
			vol_->doBuildThreadedVolume();
		} catch(...){
			return false;
		}

		printf("OmVolumeBuilder: BuildThreadedVolume() done : %.6f secs\n",
			   timer.s_elapsed());

		return true;
	}

	bool areImportFilesImages()
	{
		if( files_[0].fileName().endsWith(".h5", Qt::CaseInsensitive) ||
			files_[0].fileName().endsWith(".hdf5", Qt::CaseInsensitive)){
			return false;
		}

		return true;
	}

	Vector3i get_dims(const OmDataPath& dataset )
	{
		if(areImportFilesImages()){
			QImage img(files_[0].absoluteFilePath());
			Vector3i dims(img.width(), img.height(), files_.size());
			printf("dims are %dx%dx%d\n", DEBUGV3(dims));
			return dims;
		}

		return get_dims_hdf5(dataset);
	}

	Vector3i get_dims_hdf5(const OmDataPath& dataset )
	{
		if(files_.size() != 1){
			throw OmArgException("expected one hdf5 file");
		}

		OmIDataReader* hdf5reader =
			OmDataLayer::getReader(files_[0].filePath().toStdString(),
								   true, aff_);

		hdf5reader->open();

		//get dims of image
		Vector3i dims;
		if(hdf5reader->dataset_exists(dataset)){
			dims = hdf5reader->getChunkedDatasetDims( dataset );
		} else {
			dims = hdf5reader->getChunkedDatasetDims( OmDataPaths::getDefaultDatasetName() );
		}

		//debug(hfd5image, "dims are %i,%i,%i\n", DEBUGV3(dims));

		hdf5reader->close();

		return dims;
	}

	void UpdateMipProperties(const OmDataPath& dataset)
	{
		const Vector3i source_dims = get_dims(dataset);

		if (vol_->GetDataDimensions() != source_dims) {
			//printf("OmMipVolume::UpdateMipProperties: CHANGING VOLUME DIMENSIONS\n");

			vol_->SetDataDimensions(source_dims);
		}

		if(vol_->GetChunkDimension() % 2){
			throw OmFormatException("Chunk dimensions must be even.");
		}

		vol_->UpdateRootLevel();
	}

	QFileInfoList GetSourceFilenamesAndPaths()
	{
		return files_;
	}

	bool isSourceValid()
	{
		if( files_.empty() ){
			return false;
		}

		foreach( const QFileInfo & fi, files_ ){
			if( !fi.exists() ){
				return false;
			}
		}

		return true;
	}
};

#endif
