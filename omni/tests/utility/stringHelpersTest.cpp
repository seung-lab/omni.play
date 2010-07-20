#include "tests/utility/stringHelpersTest.h"
#include "src/utility/stringHelpers.h"

void StringHelpersTest::testCommaDeliminateNumber()
{
	assert( "10" == StringHelpers::commaDeliminateNumber( 10 ) );
}
