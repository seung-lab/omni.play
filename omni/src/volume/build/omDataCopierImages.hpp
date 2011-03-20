#ifndef OM_DATA_COPIED_IMAGES_HPP
#define OM_DATA_COPIED_IMAGES_HPP

#include "volume/build/omDataCopierBase.hpp"
#include "volume/build/omLoadImage.h"

#include <QImage>

template <typename VOL>
class OmDataCopierImages : public OmDataCopierBase<VOL> {
private:
	VOL *const vol_;
	const std::vector<QFileInfo>& files_;

	std::vector<boost::shared_ptr<QFile> > volFiles_;

public:
	OmDataCopierImages(VOL* vol,
					   const std::vector<QFileInfo>& files)
		: OmDataCopierBase<VOL>(vol)
		, vol_(vol)
		, files_(files)
	{}

protected:

	virtual void doImport()
	{
		allocateData(determineDataType());

		boost::shared_ptr<QFile> mip0volFile = volFiles_[0];

		OmLoadImage<VOL> imageLoader(vol_, mip0volFile, files_);

		for(size_t i = 0; i < files_.size(); ++i){
			const QString fnp = files_[i].absoluteFilePath();
			imageLoader.processSlice(fnp, i);
		}
	}

	OmVolDataType determineDataType()
	{
		const int depth = QImage(files_[0].absoluteFilePath()).depth();

		switch(depth){
		case 8:
			return OmVolDataType::UINT8;
		case 32:
			return OmVolDataType::UINT32;
		default:
			throw OmIoException("don't know how to import image with bpp of",
								QString::number(depth));
		}
	}

	void allocateData(const OmVolDataType type){
		volFiles_ = OmVolumeAllocater::AllocateData(vol_, type);
	}
};

#endif
