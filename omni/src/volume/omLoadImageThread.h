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

	void processSlice(const QString & fn, 
			  const int sliceNum, 
			  const Vector3i leaf_mip_dims,
			  const int numberOfBytes);
	void doProcessSlice(const QImage & img, 
			    const int sliceNum, 
			    const Vector3i leaf_mip_dims,
			    const int numberOfBytes);
};

#endif
