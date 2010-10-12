#ifndef TESTS_HPP
#define TESTS_HPP

#include "src/utility/image/omImage.hpp"
#include "src/common/omCommon.h"
#include "volume/omMipVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omChannel.h"
#include "utility/omTimer.h"
#include "datalayer/omDataPaths.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

class Tests{
public:
	Tests()
		: gen(boost::mt19937(std::time(0)))
		, runPerfTests_(ZiARG_perf)
	{
	}

	void Run()
	{
//		imageResize();
		DataToMipCoordTest();
		rounding();
		powersOf2();
		if(runPerfTests_){
			mapTests();
		}
		hdf5Paths();
		printf("done\n");
	}

private:
	boost::mt19937 gen;
	const bool runPerfTests_;

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

		// ROUNDDOWN and ROUNDUP not yet valid for negative numbers...
		/*
		std::cout << ROUNDDOWN(-8,128) << "\n";
		std::cout << -8 % 128 << "\n";
		std::cout << abs(-8) % 128 << "\n";
		assert(-128 == ROUNDDOWN(-8,128));
		assert(0 == ROUNDUP(-8,128));
		*/

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

		assert(OmMipChunkCoord::NULL_COORD ==
			   OmMipVolume::DataToMipCoord(DataCoord(-8, 0, 0), 0, chunkDims));

		const Vector3<uint64_t> chunkDims64(128,128,128);
		assert(OmMipChunkCoord(0,1,1,1) ==
			   OmMipVolume::DataToMipCoord(DataCoord(130,128,128), 0, chunkDims64));

		if(runPerfTests_){
			OmTimer timer;
			const uint64_t max = 2000000;
			for(uint64_t i = 0; i < max; ++i){
				assert(OmMipChunkCoord(2,1,1,1) ==
					   OmMipVolume::DataToMipCoord(DataCoord(512,512,512), 2, chunkDims));
			}
			std::cout << max << " conversions in " << timer.s_elapsed() << " secs\n";
		}

		printf("DataToMipCoordTest OK\n");
	}

	void powersOf2()
	{
		for(int i = 0; i < 32; ++i){
			if(std::pow(static_cast<float>(2),
						static_cast<float>(i)) != om::pow2int(i)){
				std::cout << "pow2 fail at i==" << i
						  << "; was " << om::pow2int(i)
						  << ", but should be: "
						  << std::pow(static_cast<float>(2),
									  static_cast<float>(i))
						  << "\n";
					assert(0);
			}
		}

		if(runPerfTests_){
			OmTimer timer;
			const uint64_t max = 2000000;
			uint64_t sum = 0;
			for(uint64_t i = 0; i < max; ++i){
				sum += om::pow2int(8);

			}
			std::cout << "\t" << max << " pow2 lookups in " << timer.s_elapsed() << " secs\n";

			timer.restart();
			sum = 0;
			for(uint64_t i = 0; i < max; ++i){
				sum += std::pow(static_cast<double>(2),
								static_cast<double>(8));
			}
			std::cout << "\t" << max << " std::pow in " << timer.s_elapsed() << " secs\n";
		}

		printf("powersOf2 ok\n");
	}

	void mapTests()
	{
		const uint64_t max = 2000000;

		std::map<int, int> stdMap;
		testMapSpeed(stdMap, max, "std::map");

		boost::unordered_map<int,int> boostMap;
		testMapSpeed(boostMap, max, "boost::unordered_map");
	}

	template <typename T>
	void testMapSpeed(T& map, const uint64_t max,
					  const std::string& mapType)
	{
		OmTimer timer;
		for(uint64_t i = 0; i < max; ++i){
			map[i] = i;

		}
		std::cout << "\t" << max << " inserts into " << mapType
				  << " in " << timer.s_elapsed() << " secs\n";

		timer.restart();
		double sum = 0;
		FOR_EACH(iter, map){
			const int key = iter->first;
			const int val = iter->second;
			sum += key + val;
		}
		std::cout << "\t" << max << " {key,val} walk in " << mapType
				  << " in " << timer.s_elapsed() << " secs\n";

		timer.restart();
		sum = 0;
		boost::uniform_int<> dist(0, max);
		boost::variate_generator<boost::mt19937&,
			boost::uniform_int<> > rrand(gen, dist);
		for(uint64_t i=0; i < max; ++i){
			sum += map[ rrand() ];
		}
		std::cout << "\t" << max << " rand gets in " << mapType
				  << " in " << timer.s_elapsed() << " secs\n";

	}

	void hdf5Paths()
	{
		boost::shared_ptr<OmSegmentation> seg1(new OmSegmentation(1));

		assert("segmentations/segmentation1/" ==
			   OmDataPaths::getDirectoryPath(seg1.get()));

		boost::shared_ptr<OmChannel> chann1(new OmChannel(1));
		assert("channels/channel1/" ==
			   OmDataPaths::getDirectoryPath(chann1.get()));

		printf("hdf5 tests OK\n");
	}

};

#endif
