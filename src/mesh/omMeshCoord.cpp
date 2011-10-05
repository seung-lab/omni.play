#include "omMeshCoord.h"
#include "common/omDebug.h"

OmMeshCoord::OmMeshCoord()
{
    MipChunkCoord = om::chunkCoord();
    DataValue = -1;
}

OmMeshCoord::OmMeshCoord(const om::chunkCoord & rMipChunkCoord, segId dataValue)
    : MipChunkCoord(rMipChunkCoord)
    , DataValue(dataValue)
{

}

void OmMeshCoord::operator=(const OmMeshCoord & rhs)
{
    MipChunkCoord = rhs.MipChunkCoord;
    DataValue = rhs.DataValue;
}

bool OmMeshCoord::operator==(const OmMeshCoord & rhs) const
{
    return MipChunkCoord == rhs.MipChunkCoord && DataValue == rhs.DataValue;
}

bool OmMeshCoord::operator!=(const OmMeshCoord & rhs) const
{
    return MipChunkCoord != rhs.MipChunkCoord || DataValue != rhs.DataValue;
}

/* comparitor for key usage */
bool OmMeshCoord::operator<(const OmMeshCoord & rhs) const
{
    if (MipChunkCoord != rhs.MipChunkCoord) {
        return (MipChunkCoord < rhs.MipChunkCoord);
    }

    return (DataValue < rhs.DataValue);
}

std::ostream& operator<<(std::ostream &out, const OmMeshCoord &c)
{
    out << "(" << c.DataValue << ", "
        << c.MipChunkCoord
        << ")";
    return out;
}
