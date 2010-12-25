#ifndef OM_FILE_NAMES_HPP
#define OM_FILE_NAMES_HPP

#include "common/omDebug.h"
#include "system/omProjectData.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omSegmentation.h"
#include "zi/omMutex.h"

#include <QFile>
#include <QDir>

class OmFileNames {
public:
	static QString GetProjectFileName()
	{
		const QDir& filesDir = OmProjectData::GetFilesFolderPath();
		const QString projectName("project.qt.dat");
		return filesDir.absolutePath() + QDir::separator() + projectName;
	}

	template <typename T>
	static QString GetVolPath(T* vol)
	{
		const QDir& filesDir = OmProjectData::GetFilesFolderPath();

		const QString subPath = QString("%1/")
			.arg(QString::fromStdString(vol->GetDirectoryPath()));

		if(subPath.startsWith("/")){
			throw OmIoException("not a relative path: " + subPath.toStdString());
		}

		return filesDir.absolutePath() + QDir::separator() + subPath;
	}

	template <typename T>
	static QString MakeVolPath(T* vol)
	{
		static zi::rwmutex lock;

		const QString fullPath = GetVolPath(vol);

		{
			zi::rwmutex::write_guard g(lock);

			if(!QDir(fullPath).exists()){
				if(!QDir().mkpath(fullPath)){
					throw OmIoException("could not create folder", fullPath);
				}
			}
		}

		return fullPath;
	}

	static QString GetVolSegmentsPath(OmSegmentation* vol)
	{
		const QDir filesDir(GetVolPath(vol));

		const QString subPath("segments/");

		return filesDir.absolutePath() + QDir::separator() + subPath;
	}

	static QString MakeVolSegmentsPath(OmSegmentation* vol)
	{
		static zi::rwmutex lock;

		const QString fullPath = GetVolSegmentsPath(vol);

		{
			zi::rwmutex::write_guard g(lock);
			if(!QDir(fullPath).exists()){
				if(!QDir().mkpath(fullPath)){
					throw OmIoException("could not create folder", fullPath);
				}
			}
		}

		return fullPath;
	}

	static QString GetMeshChunkFolderPath(OmSegmentation* vol,
										  const double threshold,
										  const OmMipChunkCoord& coord)
	{
		const QDir filesDir(GetVolPath(vol));

		const QString subPath =
			QString("meshes/%1/%2/%3/%4/%5/")
			.arg(QString::number(threshold, 'f', 4))
			.arg(coord.getLevel())
			.arg(coord.getCoordinateX())
			.arg(coord.getCoordinateY())
			.arg(coord.getCoordinateZ());

		return filesDir.absolutePath() + QDir::separator() + subPath;
	}

	static QString MakeMeshChunkFolderPath(OmSegmentation* vol,
										   const double threshold,
										   const OmMipChunkCoord& coord)
	{
		static zi::rwmutex lock;

		const QString fullPath = GetMeshChunkFolderPath(vol,
														threshold,
														coord);
		{
			zi::rwmutex::write_guard g(lock);
			if(!QDir(fullPath).exists()){
				if(!QDir().mkpath(fullPath)){
					throw OmIoException("could not create folder", fullPath);
				}
			}
		}

		return fullPath;
	}

	static QString GetMeshFolderPath(OmSegmentation* vol)
	{
		const QDir filesDir(GetVolPath(vol));

		const QString subPath("meshes/");

		return filesDir.absolutePath() + QDir::separator() + subPath;
	}

	static QString MakeMeshFolderPath(OmSegmentation* vol)
	{
		static zi::rwmutex lock;

		const QString fullPath = GetMeshFolderPath(vol);

		{
			zi::rwmutex::write_guard g(lock);
			if(!QDir(fullPath).exists()){
				if(!QDir().mkpath(fullPath)){
					throw OmIoException("could not create folder", fullPath);
				}
			}
		}

		return fullPath;
	}

	//TODO: cleanup!
	//ex:  /home/projectName.files/segmentations/segmentation1/0/volume.int32_t.raw
	template <typename T>
	static std::string GetMemMapFileName(T* vol, const int level)
	{
		return GetMemMapFileNameQT(vol, level).toStdString();
	}

	template <typename T>
	static QString GetMemMapFileNameQT(T* vol, const int level)
	{
		static zi::rwmutex lock;
		zi::rwmutex::write_guard g(lock);

		const QDir& filesDir = OmProjectData::GetFilesFolderPath();

		const QString subPath = QString("%1/%2/")
			.arg(QString::fromStdString(vol->GetDirectoryPath()))
			.arg(level);

		if(subPath.startsWith("/")){
			throw OmIoException("not a relative path: " + subPath.toStdString());
		}

		const QString fullPath = filesDir.absolutePath() + QDir::separator() + subPath;

		if(!QDir(fullPath).exists()){
			if(!QDir().mkpath(fullPath)){
				throw OmIoException("could not create folder", fullPath);
			}
		}

		const std::string volType = vol->getVolDataTypeAsStr();

		const QString fnp = QString("/%1/volume.%2.raw")
			.arg(fullPath)
			.arg(QString::fromStdString(volType));

		const QString fnp_clean = QDir::cleanPath(fnp);

		ZiLOG(DEBUG, io) << "file is " << fnp_clean.toStdString() << "\n";

		return fnp_clean;
	}
};

#endif
