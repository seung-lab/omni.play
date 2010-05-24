#include "omDataArchiveBoost.h"

QDataStream &operator<<(QDataStream & out, const OmIds & set )
{
	out << (quint32)set.size();

	OmIds::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		out << *iter;
	}

	return out;
}

QDataStream &operator>>(QDataStream & in, OmIds & set )
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
