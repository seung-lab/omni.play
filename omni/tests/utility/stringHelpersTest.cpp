#include "src/utility/omStringHelpers.h"
#include "tests/testUtils.hpp"
#include "tests/utility/stringHelpersTest.h"
#include "utility/omTimer.hpp"

void StringHelpersTest::testCommaDeliminateNumber(const bool perfTest)
{
    verify( "10" == om::string::humanizeNum(10));
    verify( "100" == om::string::humanizeNum(100));
    verify( "1,000" == om::string::humanizeNum(1000));
    verify( "10,000" == om::string::humanizeNum(10000));
    verify( "100,000" == om::string::humanizeNum(100000));
    verify( "1,000,000" == om::string::humanizeNum(1000000));

    verify( "2,000,000,000" == om::string::humanizeNum(2000000000));

    if(perfTest){
        OmTimer timer;
        const uint64_t max = 2000000;

        for(uint64_t i = 0; i < max; ++i){
            verify( "2,000,000,000" == om::string::humanizeNum(2000000000));
        }
        std::cout << max << " conversions in " << timer.s_elapsed() << " secs\n";
    }
}
