#ifndef OM_SIMPLE_CHUNK_H
#define OM_SIMPLE_CHUNK_H

#include "omMipChunk.h"
#include "datalayer/omDataWrapper.h"

class OmSimpleChunk : public OmMipChunk {

public:
	OmSimpleChunk(const OmMipChunkCoord &rMipCoord, OmMipVolume *pMipVolume);
	~OmSimpleChunk();

	virtual void Close();
	virtual void OpenForWrite();
	virtual void WriteVolumeData();

	int GetSize();
	virtual void ReadVolumeData();
	virtual quint32 GetVoxelValue(const DataCoord &vox);
	virtual void SetVoxelValue(const DataCoord &vox, quint32 value);

	void SetImageData(void * pImageData);

	boost::unordered_map< OmSegID, unsigned int> * RefreshDirectDataValues( OmSegmentCache *);

	virtual void * ExtractDataSlice(OmDataVolumePlane plane, int offset, Vector2<int> &sliceDims, bool fast = false);

	void *GetMeshImageData();

 private:
	OmDataWrapperPtr mpImageData;
	
	friend QDataStream &operator<<(QDataStream & out, const OmSimpleChunk & chunk );
	friend QDataStream &operator>>(QDataStream & in, OmSimpleChunk & chunk );
};

#endif
