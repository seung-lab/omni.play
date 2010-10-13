#ifndef OM_FILE_NAMES_HPP
#define OM_FILE_NAMES_HPP

#include "system/omProjectData.h"

#include <QFile>
#include <QDir>

class OmFileNames {
public:

	//TODO: cleanup!
	//ex:  /home/projectName.files/segmentations/segmentation1/0/volume.int32_t.raw
	template <typename T>
	static std::string GetMemMapFileName(T* vol, const int level)
	{
		const QDir filesDir = OmProjectData::GetFilesFolderPath();

		const QString subPath = QString("%1/%2/")
			.arg(QString::fromStdString(vol->GetDirectoryPath()))
			.arg(level);

		if(subPath.startsWith("/")){
			throw OmIoException("not a relative path: " + subPath.toStdString());
		}

		const QString fullPath = filesDir.absolutePath() + QDir::separator() + subPath;

		if(!filesDir.mkpath(subPath)){
			throw OmIoException("could not create folder " + fullPath.toStdString());
		}

		const std::string volType =vol->getVolDataTypeAsStr();

		const QString fnp = QString("/%1/volume.%2.raw")
			.arg(fullPath)
			.arg(QString::fromStdString(volType));

		const QString fnp_clean = QDir::cleanPath(fnp);

		ZiLOG(DEBUG, io) << "file is " << fnp_clean.toStdString() << "\n";

		return fnp_clean.toStdString();
	}
};

#endif
