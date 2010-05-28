#include "common/omCommon.h"
#include "system/omProjectData.h"
#include "utility/omDataArchiveBoost.h"
#include "utility/omDataArchiveMipChunk.h"
#include "utility/omHdf5Path.h"
#include "volume/omMipChunk.h"
#include "volume/omSimpleChunk.h"
#include "utility/omDataReader.h"
#include "utility/omDataWriter.h"

#include <QDataStream>

void OmDataArchiveMipChunk::ArchiveRead( const OmHdf5Path & path, OmMipChunk * chunk ) 
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

void OmDataArchiveMipChunk::ArchiveWrite( const OmHdf5Path & path, OmMipChunk * chunk ) 
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
void OmDataArchiveMipChunk::ArchiveRead( const OmHdf5Path & path, OmSimpleChunk * chunk ) 
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

void OmDataArchiveMipChunk::ArchiveWrite( const OmHdf5Path & path, OmSimpleChunk * chunk ) 
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
