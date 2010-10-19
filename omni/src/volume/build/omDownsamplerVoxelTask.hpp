#ifndef OM_DOWNSAMPLER_VOXEL_TASK
#define OM_DOWNSAMPLER_VOXEL_TASK

template <typename T>
class DownsampleVoxelTask : public zi::runnable {
private:
	OmMipVolume *const vol_;
	const std::vector<MipLevelInfo>& mips_;
	const MippingInfo& mippingInfo_;
	const OmMipChunkCoord coord_;
	const Vector3i srcChunkStartPos_;
	boost::shared_ptr<OmRawChunkCache<T> > cache_;

	std::vector<boost::shared_ptr<OmRawChunk<T> > > chunks_;

public:
	DownsampleVoxelTask(OmMipVolume* vol,
						const std::vector<MipLevelInfo>& mips,
						const MippingInfo& mippingInfo,
						const OmMipChunkCoord& coord,
						boost::shared_ptr<OmRawChunkCache<T> > cache)
		: vol_(vol)
		, mips_(mips)
		, mippingInfo_(mippingInfo)
		, coord_(coord)
		, srcChunkStartPos_(vol_->MipCoordToDataBbox(coord_, 0).getMin())
		, cache_(cache)
	{
		chunks_.resize(mippingInfo.maxMipLevel + 1);

		for(int i = 1; i <= mippingInfo_.maxMipLevel; ++i){
			const DataCoord dstCoord = coord_.Coordinate / mips_[i].factor;
			const OmMipChunkCoord coord(i, dstCoord);

			boost::shared_ptr<OmRawChunk<T> > chunk;
			cache_->Get(chunk, coord, om::BLOCKING);
			chunks_[i] = chunk;
		}
	}

	void run()
	{
		OmRawChunk<T> mip0chunk(vol_, coord_);

		const int sliceSize = 128*128;

		for(uint64_t sz=0, si=0; sz < 128; sz+=2, si+=sliceSize){
			for(uint64_t sy=0; sy < 128; sy+=2, si+=128){
				for(uint64_t sx=0; sx < 128; sx+=2, si+=2){

					const Vector3i srcVoxelPosInChunk(sx,sy,sz);

					const T srcVoxel = mip0chunk.Get(si);

					pushVoxelIntoMips(srcChunkStartPos_ + srcVoxelPosInChunk,
									  srcVoxel);
				}
			}
		}
	}

	inline uint64_t computeVoxelOffsetIntoChunk(const Vector3i& offsetVec)
	{
		return (mippingInfo_.tileSize    * offsetVec.z +
				mippingInfo_.chunkDims.x * offsetVec.y +
				offsetVec.x);
	}

	inline void pushVoxelIntoMips(const DataCoord& srcCoord, const T srcVoxel)
	{
		for(int i = 1; i <= mippingInfo_.maxMipLevel; ++i){
			if( 0 != srcCoord.z % mips_[i].factor ||
				0 != srcCoord.y % mips_[i].factor ||
				0 != srcCoord.x % mips_[i].factor ){
				return;
			}

			const DataCoord dstLocation = srcCoord / mips_[i].factor;
			const DataCoord dstCoord(dstLocation.x  % 128,
									 dstLocation.y  % 128,
									 dstLocation.z  % 128);
			const uint64_t offset = computeVoxelOffsetIntoChunk(dstCoord);
			chunks_[i]->Set(offset, srcVoxel);
		}
	}
};

#endif
