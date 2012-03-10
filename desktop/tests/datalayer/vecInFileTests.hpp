#pragma once

#include "datalayer/fs/omVecInFile.hpp"

class VecInFileTests{
public:
	VecInFileTests()
	{}

	void RunAll()
	{
		simple();
		printf("done VecInFileTests\n");
	}

private:
	void simple()
	{
		const uint32_t size = 10000;

		{
			OmVectorInFile<uint32_t> vif("/tmp/vif.test");
			std::vector<uint32_t>& v = vif.Vector();
			v.resize(size, 11);
			vif.Save();
		}

		{
			OmVectorInFile<uint32_t> vif("/tmp/vif.test");
			vif.Load();
			const std::vector<uint32_t>& v = vif.Vector();
			for(uint32_t i = 0; i < size; ++i){
				verify(11 == v[i]);
			}
		}
	}

};

