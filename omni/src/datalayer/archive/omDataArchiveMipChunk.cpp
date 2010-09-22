#include "common/omCommon.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "datalayer/archive/omDataArchiveMipChunk.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataWriter.h"
#include "system/omProjectData.h"
#include "volume/omMipChunk.h"

#include <QDataStream>

void OmDataArchiveMipChunk::ArchiveRead( const OmDataPath & path, OmMipChunk * chunk )
{
	int size;
	OmDataWrapperPtr dw = OmProjectData::GetProjectDataReader()->readDataset(path, &size);

	QByteArray ba = QByteArray::fromRawData( dw->getPtr<char>(), size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);
	in >> (*chunk);
}

void OmDataArchiveMipChunk::ArchiveWrite( const OmDataPath & path, OmMipChunk * chunk )
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);
	out << (*chunk);

	OmProjectData::GetDataWriter()->writeDataset( path,
						      ba.size(),
						      OmDataWrapperRaw(ba.data()));
}

QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk )
{
	//meta data
	out << chunk.mDirectlyContainedValues;
	out << chunk.mModifiedVoxelValues;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk )
{
	in >> chunk.mDirectlyContainedValues;
	in >> chunk.mModifiedVoxelValues;

	return in;
}
