#include "omDataArchiveValue.h"
#include <QDataStream>

void OmDataArchiveValue::ArchiveRead( OmHdf5Path path, QHash<SEGMENT_DATA_TYPE, OmId> & page )
{
	int size;
	char* p_data = (char*) OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( p_data, size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);
	in >> page;

	delete p_data;
}

void OmDataArchiveValue::ArchiveWrite( OmHdf5Path path, QHash<SEGMENT_DATA_TYPE, OmId> & page )
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);
	out << page;
	
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   ba.data() );
}
