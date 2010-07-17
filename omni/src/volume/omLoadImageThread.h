#ifndef OM_LOAD_IMG_THREAD_H
#define OM_LOAD_IMG_THREAD_H

#include "common/omCommon.h"
#include <QRunnable>
#include <QImage>

class OmMipVolume;

class OmLoadImageThread : public QRunnable
{
 public:
	OmLoadImageThread(OmMipVolume * p);
        void run();

 private:
	OmMipVolume *const mMipVolume;
	
	const Vector3i m_leaf_mip_dims;
	const int m_numberOfBytes;

	void processSlice(const QString &, const int);
	void doProcessSlice(const QImage &, const int);
};

#endif
