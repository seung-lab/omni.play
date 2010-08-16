#ifndef OM_LOAD_IMG_THREAD_H
#define OM_LOAD_IMG_THREAD_H

#include "common/omCommon.h"
#include "volume/build/omVolumeImporter.hpp"

#include <QImage>

class OmMipVolume;

template <typename T>
class OmLoadImage
{
 public:
	OmLoadImage(OmVolumeImporter<T>* importer, T * p);

	void processSlice(const QString &, const int);

 private:
	OmVolumeImporter<T> *const importer;
	T *const mMipVolume;

	const Vector3i m_leaf_mip_dims;
	const int m_numberOfBytes;

	QString mMsg;
	const int mTotalNumImages;

	void doProcessSlice(const QImage &, const int);
};

#endif
