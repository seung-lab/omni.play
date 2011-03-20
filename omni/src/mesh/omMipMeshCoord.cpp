
#include "omMipMeshCoord.h"
#include "common/omDebug.h"

OmMipMeshCoord::OmMipMeshCoord()
{
	MipChunkCoord = OmChunkCoord();
	DataValue = -1;
}

OmMipMeshCoord::OmMipMeshCoord(const OmChunkCoord & rMipChunkCoord, OmSegID dataValue)
	: MipChunkCoord(rMipChunkCoord)
	, DataValue(dataValue)
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
	if (MipChunkCoord != rhs.MipChunkCoord) {
		return (MipChunkCoord < rhs.MipChunkCoord);
	}

	return (DataValue < rhs.DataValue);
}

std::ostream& operator<<(std::ostream &out, const OmMipMeshCoord &c) {
	out << "(" << c.DataValue << ", "
		<< c.MipChunkCoord
		<< ")";
	return out;
}
