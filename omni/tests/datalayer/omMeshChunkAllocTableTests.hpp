#ifndef OM_MESH_CHUNK_ALLOC_TABLE_TESTS_HPP
#define OM_MESH_CHUNK_ALLOC_TABLE_TESTS_HPP

#include "mesh/io/v2/omMeshChunkAllocTable.hpp"
#include "tests/testUtils.hpp"

class OmMeshChunkAllocTableTests{
public:
	OmMeshChunkAllocTableTests()
	{}

	void RunAll()
	{
		simple();
		printf("done in OmMeshChunkAllocTableTests\n");
	}

private:
	void simple()
	{
		OmSegmentation seg(1);
		OmMipChunkCoord coord(0,0,0,0);
		OmMipChunkPtr chunk(new OmMipChunk(coord, &seg));

		const OmSegID maxSeg = 100;

		{
			OmMeshChunkAllocTableV2 fat(&seg, chunk);
			std::vector<OmMeshDataEntry>& table = *(fat.table_);

			table.resize(maxSeg);

			for(OmSegID i = maxSeg-1; i > 0; --i){
				OmMeshDataEntry e;
				e.segID = i;
				table[i] = e;
			}
			zi::random_shuffle(table.begin(), table.end());
			zi::sort(table.begin(), table.end(),
					 OmMeshChunkAllocTableV2::compareBySegID);
		}

		{
			// test rereading file
			OmMeshChunkAllocTableV2 fat(&seg, chunk);

			for(OmSegID i = 1; i < maxSeg; ++i){
				const OmMeshDataEntry& e = fat.Get(i);
				verify(i == e.segID);
			}

			try{
				fat.Get(maxSeg);
				verify(0);
			}catch(const OmIoException& e){
				// good!
			}catch(...){
				verify(0);
			}

			try{
				fat.Get(maxSeg*2);
				verify(0);
			}catch(const OmIoException& e){
				// good!
			}catch(...){
				verify(0);
			}
		}
		{
			OmMeshChunkAllocTableV2 fat(&seg, chunk);
			OmMeshDataEntry e;
			e.segID = 10;
			fat.Set(e);
		}
	}
};

#endif
