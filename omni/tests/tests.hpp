#ifndef TESTS_HPP
#define TESTS_HPP

#include "src/utility/image/omImage.hpp"
#include "src/common/omCommon.h"
#include "volume/omMipVolume.h"
#include "utility/omTimer.h"

class Tests{
public:
	Tests(){}

	void Run()
	{
//		imageResize();
		DataToMipCoordTest();
		rounding();
		printf("done\n");
	}

private:
	void imageResize()
	{
		uint32_t data[] = {
			0,1,
			2,3,
			4,5,
			6,7
		};

		OmImage<uint32_t, 3> d(OmExtents
							   [2]
							   [2]
							   [2],
							   data);

		assert(8 == d.size());
		const uint32_t* ptr = d.getScalarPtr();
		for(uint32_t i = 0; i <d.size(); ++i){
			printf("%d: %d\n", i, ptr[i]);
			assert(ptr[i] == i);
		}

		d.resize(Vector3i(4,4,4));

		const uint32_t* ptr2 = d.getScalarPtr();
		for(uint32_t i = 0; i <d.size(); ++i){
			printf("%d: %d\n", i, ptr2[i]);
		}

	}

	void rounding()
	{
		assert( 10 == ROUNDUP(1,10) );
		assert( 0 == ROUNDUP(0,10));

		assert(128 == ROUNDUP(127,128));

		assert(0 == ROUNDDOWN(5,10));
		assert(10 == ROUNDDOWN(11,10));
		assert(128 == ROUNDDOWN(129,128));

		printf("rounding OK\n");
	}

	void DataToMipCoordTest()
	{
		const Vector3i chunkDims(128,128,128);

		assert(OmMipChunkCoord(0,0,0,0) ==
			   OmMipVolume::DataToMipCoord(DataCoord(1,1,1), 0, chunkDims));

		assert(OmMipChunkCoord(0,1,1,1) ==
			   OmMipVolume::DataToMipCoord(DataCoord(129,129,129), 0, chunkDims));

		assert(OmMipChunkCoord(1,0,0,0) ==
			   OmMipVolume::DataToMipCoord(DataCoord(255,255,255), 1, chunkDims));

		assert(OmMipChunkCoord(1,1,1,1) ==
			   OmMipVolume::DataToMipCoord(DataCoord(256,256,256), 1, chunkDims));

		assert(OmMipChunkCoord(2,0,0,0) ==
			   OmMipVolume::DataToMipCoord(DataCoord(511,511,511), 2, chunkDims));

		assert(OmMipChunkCoord(2,1,1,1) ==
			   OmMipVolume::DataToMipCoord(DataCoord(512,512,512), 2, chunkDims));

/*
		OmTimer timer;
		const uint64_t max = 2000000;
		for(uint64_t i = 0; i < max; ++i){
			assert(OmMipChunkCoord(2,1,1,1) ==
				   OmMipVolume::DataToMipCoord(DataCoord(512,512,512), 2, chunkDims));
		}
		std::cout << max << " conversions in " << timer.s_elapsed() << " secs\n";
*/

		printf("DataToMipCoordTest OK\n");
	}
};

#endif
