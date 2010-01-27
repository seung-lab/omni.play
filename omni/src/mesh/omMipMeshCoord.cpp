
#include "omMipMeshCoord.h"
#include "system/omDebug.h"

OmMipMeshCoord::OmMipMeshCoord()
{
	MipChunkCoord = OmMipChunkCoord();
	DataValue = -1;
}

OmMipMeshCoord::OmMipMeshCoord(const OmMipChunkCoord & rMipChunkCoord, SEGMENT_DATA_TYPE dataValue)
:MipChunkCoord(rMipChunkCoord), DataValue(dataValue)
{

}

OmMipMeshCoord::OmMipMeshCoord(const tuple < int, int, int, int >&rMipChunkCoord, SEGMENT_DATA_TYPE dataValue)
{

	MipChunkCoord.Level = rMipChunkCoord.get < 0 > ();
	MipChunkCoord.Coordinate.x = rMipChunkCoord.get < 1 > ();
	MipChunkCoord.Coordinate.y = rMipChunkCoord.get < 2 > ();
	MipChunkCoord.Coordinate.z = rMipChunkCoord.get < 3 > ();
	DataValue = dataValue;
}

void
 OmMipMeshCoord::operator=(const OmMipMeshCoord & rhs)
{
	MipChunkCoord = rhs.MipChunkCoord;
	DataValue = rhs.DataValue;
}

bool OmMipMeshCoord::operator==(const OmMipMeshCoord & rhs) const const
{
	return MipChunkCoord == rhs.MipChunkCoord && DataValue == rhs.DataValue;
}

bool OmMipMeshCoord::operator!=(const OmMipMeshCoord & rhs) constconst
{
	return MipChunkCoord != rhs.MipChunkCoord || DataValue != rhs.DataValue;
}

/* comparitor for key usage */
bool OmMipMeshCoord::operator<(const OmMipMeshCoord & rhs) constconst
{
	if (MipChunkCoord != rhs.MipChunkCoord)
		return (MipChunkCoord < rhs.MipChunkCoord);
	return (DataValue < rhs.DataValue);
}

ostream & operator<<(ostream & out, const OmMipMeshCoord & in)
{
	out << in.MipChunkCoord << " { " << (int)in.DataValue << " } ";
}
