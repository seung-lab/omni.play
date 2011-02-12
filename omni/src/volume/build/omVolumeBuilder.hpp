#ifndef OM_VOLUME_BUILDER_HPP
#define OM_VOLUME_BUILDER_HPP

#include "utility/sortHelpers.h"
#include "volume/build/omVolumeBuilderHdf5.hpp"
#include "volume/build/omVolumeBuilderImages.hpp"
#include "volume/build/omVolumeBuilderWatershed.hpp"

#include <QFileInfo>

template <typename VOL>
class OmVolumeBuilder {
private:
	VOL *const vol_;
	std::vector<QFileInfo> files_;
	const QString hdf5path_;

	enum ImportType {
		HDF5,
		IMAGES,
		WATERSHED,
		UNKNOWN
	};
	ImportType importType_;

public:
	OmVolumeBuilder(VOL* vol,
					const std::vector<QFileInfo>& files,
					const QString& hdf5path)
		: vol_(vol)
		, files_(files)
		, hdf5path_(hdf5path)
		, importType_(UNKNOWN)
	{
		setup();
	}

	OmVolumeBuilder(VOL* vol,
					const std::vector<QFileInfo>& files)
		: vol_(vol)
		, files_(files)
		, hdf5path_("")
		, importType_(UNKNOWN)
	{
		setup();
	}

	void Build()
	{
		boost::shared_ptr<OmVolumeBuilderBase<VOL> > builder;

		switch(importType_){
		case HDF5:
			builder = boost::make_shared<OmVolumeBuilderHdf5<VOL> >(vol_,
																	files_[0],
																	hdf5path_);
			break;
		case IMAGES:
			builder = boost::make_shared<OmVolumeBuilderImages<VOL> >(vol_,
																	  files_);
			break;
		case WATERSHED:
			builder = boost::make_shared<OmVolumeBuilderWatershed<VOL> >(vol_,
																		 files_[0]);
			break;
		default:
			throw OmArgException("unknown type");
		};

		builder->Build();
	}

	void Build(const om::AffinityGraph aff)
	{
		if(HDF5 != importType_){
			throw OmArgException("first file to import is not HDF5");
		}

		OmVolumeBuilderHdf5<VOL> builder(vol_, files_[0], hdf5path_, aff);
		builder.Build();
	}

	void BuildWatershed()
	{
		if(WATERSHED != importType_){
			throw OmArgException("first file to import is not Watershed");
		}

		OmVolumeBuilderWatershed<VOL> builder(vol_, files_[0]);
		builder.Build();
	}

private:
	void setup()
	{
		sortNaturally();
		setImportType();
		isSourceValid();
	}

	void sortNaturally(){
		SortHelpers::SortNaturally(files_);
	}

	void setImportType()
	{
		if(files_.empty()){
			throw OmIoException("no source files");
		}

		const QString fnp = files_[0].fileName();

		if(fnp.endsWith(".h5", Qt::CaseInsensitive) ||
		   fnp.endsWith(".hdf5", Qt::CaseInsensitive))
		{
			importType_ = HDF5;
			return;
		}

		if(fnp.endsWith(".watershed", Qt::CaseInsensitive)){
			importType_ = WATERSHED;
			return;
		}

		importType_ = IMAGES;
	}

	void isSourceValid()
	{
		if(files_.empty()){
			throw OmIoException("no source files");
		}

		if(HDF5 == importType_ || WATERSHED == importType_){
			if(1 != files_.size()){
				throw OmIoException("only import one hdf5/watershed file at a time");
			}
		}

		foreach(const QFileInfo& file, files_){
			if(!file.exists()){
				throw OmIoException("source file not found", file.fileName());
			}
		}
	}
};

#endif
