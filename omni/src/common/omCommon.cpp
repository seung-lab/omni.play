#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "common/omString.hpp"

#include <QTextStream>

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

bool operator<(const OmColor& a, const OmColor& b)
{
	if(a.red != b.red){
		return a.red < b.red;
	}

	if(a.green != b.green){
		return a.green < b.green;
	}

	return a.blue < b.blue;
}

QTextStream &operator<<(QTextStream& out, const OmColor& c)
{
	out << c.red << "\t";
	out << c.green << "\t";
	out << c.blue;
	return out;
}

std::ostream& operator<<(std::ostream &out, const OmSegIDsSet& in)
{
	const std::string joined = om::string::join(in);

	out << "[" << joined << "]";
	return out;
}
