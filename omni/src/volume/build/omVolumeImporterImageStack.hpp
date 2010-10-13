#ifndef OM_VOLUME_IMPORTER_IMAGE_STACK_HPP
#define OM_VOLUME_IMPORTER_IMAGE_STACK_HPP

#include <QImage>

template <typename VOL>
class OmVolumeImporterImageStack{
private:
	VOL *const vol_;

public:
	OmVolumeImporterImageStack(VOL* vol)
		: vol_(vol)
	{}

	bool Import()
	{
		OmLoadImage<VOL> imageLoader(vol_);
		for( int i = 0; i < vol_->mSourceFilenamesAndPaths.size(); ++i){
			const QString fnp = vol_->mSourceFilenamesAndPaths[i].absoluteFilePath();
			imageLoader.processSlice(fnp, i);
		}
		return true;
	}

	OmVolDataType DetermineDataType()
	{
		const int depth = QImage(vol_->mSourceFilenamesAndPaths[0].absoluteFilePath()).depth();

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
