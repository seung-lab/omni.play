#include "omCommon.h"
	      
uint qHash(const SpaceCoord& c)
{
	return qHash( QString( "%1_%2_%3" )
		      .arg(c.x)
		      .arg(c.y)
		      .arg(c.z) );
}
