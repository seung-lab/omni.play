#ifndef OM_MIP_VOLUME_ARCHIVE_H
#define OM_MIP_VOLUME_ARCHIVE_H

#include "volume/omMipVolume.h"
#include "volume/omVolCoordsMipped.hpp"
#include "volume/omVolumeTypes.hpp"

#include <QDataStream>

class OmMipVolumeArchive {
public:
	template <typename VOL>
	static void Store(QDataStream& out, const VOL& v)
	{
		out << v.mId;
		out << v.mCustomName;
		out << v.mNote;

		out << v.Coords();

		out << v.mBuildState;

		const QString type =
			OmVolumeTypeHelpers::GetTypeAsQString(v.mVolDataType);
		out << type;
		std::cout << "saved type as " << type.toStdString() << "\n";
	}

	template <typename VOL>
	static void Load(QDataStream& in, VOL& v)
	{
		in >> v.mId;
		in >> v.mCustomName;
		in >> v.mNote;

		in >> v.Coords();

		in >> v.mBuildState;

		QString volDataType;
		in >> volDataType;
		v.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);
	}
};

QDataStream& operator<<(QDataStream& out, const OmMipVolCoords& c);
QDataStream& operator>>(QDataStream& in, OmMipVolCoords& c);

#endif
