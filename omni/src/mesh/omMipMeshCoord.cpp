
#include "omMipMeshCoord.h"
#include "common/omDebug.h"

OmMipMeshCoord::OmMipMeshCoord()
{
	MipChunkCoord = OmMipChunkCoord();
	DataValue = -1;
}

OmMipMeshCoord::OmMipMeshCoord(const OmMipChunkCoord & rMipChunkCoord, SEGMENT_DATA_TYPE dataValue)
:MipChunkCoord(rMipChunkCoord), DataValue(dataValue)
{

}

void OmMipMeshCoord::operator=(const OmMipMeshCoord & rhs)
{
	MipChunkCoord = rhs.MipChunkCoord;
	DataValue = rhs.DataValue;
}

bool OmMipMeshCoord::operator==(const OmMipMeshCoord & rhs) const
{
	return MipChunkCoord == rhs.MipChunkCoord && DataValue == rhs.DataValue;
}

bool OmMipMeshCoord::operator!=(const OmMipMeshCoord & rhs) const
{
	return MipChunkCoord != rhs.MipChunkCoord || DataValue != rhs.DataValue;
}

/* comparitor for key usage */
bool OmMipMeshCoord::operator<(const OmMipMeshCoord & rhs) const
{
	if (MipChunkCoord != rhs.MipChunkCoord)
		return (MipChunkCoord < rhs.MipChunkCoord);
	return (DataValue < rhs.DataValue);
}

ostream & operator<<(ostream & out, const OmMipMeshCoord & in)
{
	out << in.MipChunkCoord << " { " << (int)in.DataValue << " } ";
	return out;
}
