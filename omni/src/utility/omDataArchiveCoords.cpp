#include "utility/omDataArchiveCoords.h"
#include "utility/omDataArchiveVmml.h"
#include "view2d/omTileCoord.h"
#include "project/omProject.h"
#include "system/omProjectData.h"

QDataStream &operator<<(QDataStream & out, const OmMipSegmentDataCoord & c )
{
 	out << c.MipChunkCoord;
	out << c.DataValue;
	return out;
}

QDataStream &operator>>(QDataStream & in, OmMipSegmentDataCoord & c )
{
 	in >> c.MipChunkCoord;
	in >> c.DataValue;
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmMipMeshCoord & c )
{
 	out << c.MipChunkCoord;
	out << c.DataValue;
	return out;
}

QDataStream &operator>>(QDataStream & in, OmMipMeshCoord & c )
{
 	in >> c.MipChunkCoord;
	in >> c.DataValue;
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmMipChunkCoord & c )
{
	out << c.Level;
	out << c.Coordinate;
	return out;
}

QDataStream &operator>>(QDataStream & in, OmMipChunkCoord & c )
{
	in >> c.Level;
	in >> c.Coordinate;
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmTileCoord & c )
{
	out << c.Level;
	out << c.Coordinate;
	return out;
}

QDataStream &operator>>(QDataStream & in, OmTileCoord & c )
{
	in >> c.Level;
	in >> c.Coordinate;
	return in;
}
