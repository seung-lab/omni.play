#include "datalayer/archive/omDataArchiveBoost.h"
#include <QDataStream>

QDataStream &operator<<(QDataStream & out, const OmIDsSet & set )
{
	out << (quint32)set.size();

	OmIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		out << *iter;
	}

	return out;
}

QDataStream &operator>>(QDataStream & in, OmIDsSet & set )
{
	quint32 size;
	in >> size;

	OmId id;
	for( quint32 i = 0; i < size; ++i ){
		in >> id;
		set.insert(id);
	}

	return in;
}
