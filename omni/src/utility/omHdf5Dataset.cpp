#include "omHdf5Dataset.h"


OmHdf5DataSet::OmHdf5DataSet (string inname, int insize, const void* indata) 
	: name(inname), size(insize), data(indata) 
{
}
