#include "common/omCommon.h"
#include "system/omProjectData.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "datalayer/archive/omDataArchiveMipChunk.h"
#include "datalayer/omDataPath.h"
#include "volume/omMipChunk.h"
#include "volume/omSimpleChunk.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"

#include <QDataStream>

void OmDataArchiveMipChunk::ArchiveRead( const OmDataPath & path, OmMipChunk * chunk ) 
{
	int size;
	char* p_data = (char*) OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( p_data, size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);
	in >> (*chunk);

	delete p_data;
}

void OmDataArchiveMipChunk::ArchiveWrite( const OmDataPath & path, OmMipChunk * chunk ) 
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);
	out << (*chunk);
	
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   ba.data() );
}
void OmDataArchiveMipChunk::ArchiveRead( const OmDataPath & path, OmSimpleChunk * chunk ) 
{
	int size;
	char* p_data = (char*) OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( p_data, size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);
	in >> (*chunk);

	delete p_data;
}

void OmDataArchiveMipChunk::ArchiveWrite( const OmDataPath & path, OmSimpleChunk * chunk ) 
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);
	out << (*chunk);
	
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   ba.data() );
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

QDataStream &operator<<(QDataStream & out, const OmSimpleChunk & chunk )
{
	//meta data
	out << chunk.mDirectlyContainedValues;
	out << chunk.mModifiedVoxelValues;	

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSimpleChunk & chunk )
{
	in >> chunk.mDirectlyContainedValues;
	in >> chunk.mModifiedVoxelValues;	

	return in;
}
