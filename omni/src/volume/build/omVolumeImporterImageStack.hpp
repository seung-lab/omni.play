#ifndef OM_VOLUME_IMPORTER_IMAGE_STACK_HPP
#define OM_VOLUME_IMPORTER_IMAGE_STACK_HPP

#include <QImage>

template <typename VOL>
class OmVolumeImporterImageStack{
private:
	VOL *const vol_;
	const QFileInfoList& files_;

public:
	OmVolumeImporterImageStack(VOL* vol)
		: vol_(vol)
		, files_(vol_->mSourceFilenamesAndPaths)
	{
		if(0 == files_.size()){
			throw OmIoException("no files!");
		}
	}

	bool Import(boost::shared_ptr<QFile> mip0volFile)
	{
		OmLoadImage<VOL> imageLoader(vol_, mip0volFile);

		for( int i = 0; i < files_.size(); ++i){
			const QString fnp = files_[i].absoluteFilePath();
			imageLoader.processSlice(fnp, i);
		}

		return true;
	}

	OmVolDataType DetermineDataType()
	{
		const int depth = QImage(files_[0].absoluteFilePath()).depth();

		switch(depth){
		case 8:
			return OmVolDataType::UINT8;
		case 32:
			return OmVolDataType::UINT32;
		default:
			printf("image depth is %d; aborting...\n", depth);
			throw OmIoException("don't know how to import image");
		}
	}
};

#endif
