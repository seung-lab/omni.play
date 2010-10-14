#ifndef OM_DOWNSAMPLER_VOXEL_TASK
#define OM_DOWNSAMPLER_VOXEL_TASK



template <typename T>
class DownsampleVoxelTask : public zi::runnable {
private:
	OmMipVolume *const vol_;
	const std::vector<MipLevelInfo<T> >& mips_;
	const MippingInfo& mippingInfo_;
	const OmMipChunkCoord coord_;
	const Vector3i srcChunkStartPos_;
	const uint64_t srcChunkOffset_;

public:
	DownsampleVoxelTask(OmMipVolume* vol,
						const std::vector<MipLevelInfo<T> >& mips,
						const MippingInfo& mippingInfo,
						const OmMipChunkCoord& coord)
		: vol_(vol)
		, mips_(mips)
		, mippingInfo_(mippingInfo)
		, coord_(coord)
		, srcChunkStartPos_(vol_->MipCoordToDataBbox(coord_, 0).getMin())
		, srcChunkOffset_(computeChunkOffsetIntoVolume(0, coord.Coordinate))
	{}

	void run()
	{
		T *const chunkStart = mips_[0].data + srcChunkOffset_;

		const int sliceSize = 128*128;

		for(uint64_t sz=0, si=0; sz < 128; sz+=2, si+=sliceSize){
			for(uint64_t sy=0; sy < 128; sy+=2, si+=128){
				for(uint64_t sx=0; sx < 128; sx+=2, si+=2){

					const Vector3i srcVoxelPosInChunk(sx,sy,sz);

					const T srcVoxel = chunkStart[si];

					pushVoxelIntoMips(srcChunkStartPos_ + srcVoxelPosInChunk,
									  srcVoxel);
				}
			}
		}
	}

	inline uint64_t computeVoxelOffsetIntoVolume(const DataCoord& coord,
												 const int level)
	{
		const DataCoord chunkPos(coord.x / mippingInfo_.chunkDims.x,
								 coord.y / mippingInfo_.chunkDims.y,
								 coord.z / mippingInfo_.chunkDims.z);

		const uint64_t chunkOffsetFromVolStart =
			computeChunkOffsetIntoVolume(level, chunkPos);

		const Vector3i voxelOffset = coord - chunkPos*mippingInfo_.chunkDims;

		return chunkOffsetFromVolStart + computeVoxelOffsetIntoChunk(voxelOffset);
	}

	inline uint64_t computeChunkOffsetIntoVolume(const int level,
												 const Vector3i& chunkPos)
	{
		return (mips_[level].volSlabSize * chunkPos.z +
				mips_[level].volRowSize  * chunkPos.y +
				mippingInfo_.chunkSize   * chunkPos.x);
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

			const DataCoord dstCoord = srcCoord / mips_[i].factor;
			const uint64_t dstOffset = computeVoxelOffsetIntoVolume(dstCoord, i);
			mips_[i].data[dstOffset] = srcVoxel;
		}
	}
};

#endif
