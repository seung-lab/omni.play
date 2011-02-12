#include "tests/utility/stringHelpersTest.h"
#include "src/utility/omStringHelpers.h"

void StringHelpersTest::testCommaDeliminateNumber()
{
	assert( "10" == OmStringHelpers::CommaDeliminateNum( 10 ) );
}
