#include "common/omCommon.h"

std::ostream& operator<<(std::ostream &out, const OmColor& c)
{
	out << "[r" << (int)c.red
		<< ",g" << (int)c.green
		<< ",b" << (int)c.blue
		<< "]";
	return out;
}

std::ostream& operator<<(std::ostream &out, const OmColorRGBA& c)
{
	out << "[r" << (int)c.red
		<< ",g" << (int)c.green
		<< ",b" << (int)c.blue
		<< ",a" << (int)c.alpha
		<< "]";
	return out;
}
