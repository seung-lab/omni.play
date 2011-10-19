#include "meshCoord.h"
#include "common/debug.h"

meshCoord::meshCoord()
{
    MipChunkCoord = coords::chunkCoord();
    DataValue = -1;
}

meshCoord::meshCoord(const coords::chunkCoord & rMipChunkCoord, common::segId dataValue)
    : MipChunkCoord(rMipChunkCoord)
    , DataValue(dataValue)
{

}

void meshCoord::operator=(const meshCoord & rhs)
{
    MipChunkCoord = rhs.MipChunkCoord;
    DataValue = rhs.DataValue;
}

bool meshCoord::operator==(const meshCoord & rhs) const
{
    return MipChunkCoord == rhs.MipChunkCoord && DataValue == rhs.DataValue;
}

bool meshCoord::operator!=(const meshCoord & rhs) const
{
    return MipChunkCoord != rhs.MipChunkCoord || DataValue != rhs.DataValue;
}

/* comparitor for key usage */
bool meshCoord::operator<(const meshCoord & rhs) const
{
    if (MipChunkCoord != rhs.MipChunkCoord) {
        return (MipChunkCoord < rhs.MipChunkCoord);
    }

    return (DataValue < rhs.DataValue);
}

std::ostream& operator<<(std::ostream &out, const meshCoord &c)
{
    out << "(" << c.DataValue << ", "
        << c.MipChunkCoord
        << ")";
    return out;
}
