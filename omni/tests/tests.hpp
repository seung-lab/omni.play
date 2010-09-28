#ifndef TESTS_HPP
#define TESTS_HPP

#include "src/utility/image/omImage.hpp"
#include "src/common/omCommon.h"

class Tests{
public:
	Tests(){}

	void Run()
	{
		imageResize();
		printf("OK!\n");
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
};

#endif
