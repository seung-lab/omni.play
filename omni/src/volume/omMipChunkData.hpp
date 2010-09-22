#ifndef OM_MIP_CHUNK_DATA_HPP
#define OM_MIP_CHUNK_DATA_HPP

template <typename T>
class OmMipChunkData {
public:
	OmMipChunkData();

	T getMin(){ return mData->getMin(); }
	T getMax(){ return mData->getMax(); }


private:
	OmImage<T,3> mData;
};

#endif
