#ifndef OM_DOWNSAMPLER_HPP
#define OM_DOWNSAMPLER_HPP

#include "volume/omMipVolume.h"

template <typename T>
class OmDownsampler {
private:
	struct MipLevelInfo {
		T* data;
		uint64_t factor;
		Vector3<uint64_t> volDims;

		uint64_t maxAllowedIndex;
		uint64_t volSlabSize;
		uint64_t volSliceSize;
		uint64_t volRowSize;
		uint64_t totalVoxels;
	};

	OmMipVolume *const vol_;
	std::vector<MipLevelInfo> mips_;
	const int maxMipLevel_;

	const Vector3<uint64_t> chunkDims_;
	const uint64_t chunkSize_;
	const uint64_t tileSize_;

public:
	OmDownsampler(OmMipVolume* vol)
		: vol_(vol)
		, maxMipLevel_(vol_->GetRootMipLevel())
		, chunkDims_(vol_->GetChunkDimensions())
		, chunkSize_(chunkDims_.x * chunkDims_.y * chunkDims_.z)
		, tileSize_(chunkDims_.x * chunkDims_.y)
	{
		mips_.resize(maxMipLevel_ + 1);

		for(int i=0; i <= maxMipLevel_; ++i){
			mips_[i].data = boost::get<T*>(vol_->getVolData()->GetVolPtr(i));
			mips_[i].factor = OMPOW(2, i);

			const Vector3i dims = vol_->getDimsRoundedToNearestChunk(i);
			mips_[i].volDims = dims;
			mips_[i].volSlabSize = dims.x * dims.y * chunkDims_.z;
			mips_[i].volSliceSize = dims.x * dims.y;
			mips_[i].volRowSize = dims.x * chunkDims_.y * chunkDims_.z;
			mips_[i].totalVoxels = dims.x * dims.y * dims.z;
		}
	}

	void DownsampleChooseOneVoxelOfNine()
	{
		printf("downsampling...\n");
		for(uint64_t sz=0; sz < mips_[0].volDims.z; sz+=2){
			for(uint64_t sy=0; sy < mips_[0].volDims.y; sy+=2){
				for(uint64_t sx=0; sx < mips_[0].volDims.x; sx+=2){
					pushVoxelIntoMips(DataCoord(sx, sy, sz));
				}
			}
		}
		printf("\t downsampling done\n");
	}

	uint64_t getOffsetIntoVolume(const DataCoord& coord, const int level,
								 const bool debug = false)
	{
		const DataCoord chunkPos =
			OmMipVolume::DataToMipCoord(coord, level, chunkDims_).Coordinate;

		if(debug){
			std::cout << "chunkPos is " << chunkPos << "\n";
		}

		const uint64_t chunkOffsetFromVolStart =
			mips_[level].volSlabSize * chunkPos.z +
			mips_[level].volRowSize  * chunkPos.y +
			chunkSize_ * chunkPos.x;

		const Vector3i offsetVec = coord - chunkPos*chunkDims_;
		if(debug){
			std::cout << "offsetVec is " << offsetVec << "\n";
		}

		const uint64_t offsetIntoChunk =
			tileSize_ * offsetVec.z +
			chunkDims_.x * offsetVec.y +
			offsetVec.x;

		return chunkOffsetFromVolStart + offsetIntoChunk;
	}

	void pushVoxelIntoMips(const DataCoord& srcCoord)
	{
		for(int i = 1; i <= maxMipLevel_; ++i){
			if( 0 != srcCoord.z % mips_[i].factor ||
				0 != srcCoord.y % mips_[i].factor ||
				0 != srcCoord.x % mips_[i].factor ){
				return;
			}

			const DataCoord dstCoord = srcCoord / mips_[i].factor;

			const uint64_t dstOffset = getOffsetIntoVolume(dstCoord, i);
			const uint64_t srcOffset = getOffsetIntoVolume(srcCoord, 0);

			if( dstOffset >= mips_[i].totalVoxels ||
				srcOffset >= mips_[0].totalVoxels ){

				std::cout << "src coord: " << srcCoord
						  << ", dst coord: " << dstCoord << "\n";
				std::cout << "\tsrc offset: " << srcOffset
						  << ", dst offset: " << dstOffset << "\n";
				std::cout << "\tsrc max offset: " << mips_[0].totalVoxels << "\n"
						  << ", dst max offset: " << mips_[i].totalVoxels << "\n";
				std::cout << "src\n********\n" << getOffsetIntoVolume(srcCoord, 0, true);
				std::cout << "\ndst\n********\n" << getOffsetIntoVolume(dstCoord, i, true);
				fflush(stdout);
				assert(0);
			}

			mips_[i].data[dstOffset] = mips_[0].data[srcOffset];
		}

	}
};

#endif
