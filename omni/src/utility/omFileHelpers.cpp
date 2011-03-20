#include "common/omDebug.h"
#include "utility/omFileHelpers.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

bool OmFileHelpers::isFileReadOnly(const std::string& fileNameAndPath)
{
	QFileInfo file(QString::fromStdString(fileNameAndPath));
	if (file.exists() && !file.isWritable() ){
		return true;
	}

	return false;
 }

void OmFileHelpers::RemoveDir(const QString &folder)
{
	QDir filesDir(folder);
	if(filesDir.exists()){
		printf("removing folder %s...", qPrintable(folder));
		fflush(stdout);
		if(removeDir(folder)){
			printf("done!\n");
		} else {
			throw OmIoException("could not remove folder", folder);
		}
	}
}

/*!
   Delete a directory along with all of its contents.

   \param dirName Path of directory to remove.
   \return true on success; false on error.

   from http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
*/
bool OmFileHelpers::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot |
													QDir::System |
													QDir::Hidden  |
													QDir::AllDirs |
													QDir::Files,
													QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

void OmFileHelpers::MoveFile(const QString& from_fnp, const QString& to_fnp)
{
	QFile file(from_fnp);
	if(!file.exists()){
		throw OmIoException("could not find file", from_fnp);
	}

	if(QFile::exists(to_fnp)){
		if(!QFile::remove(to_fnp)){
			throw OmIoException("could not remove previous file", to_fnp);
		}
	}

	if(!file.rename(to_fnp)){
		throw OmIoException("could not rename file to", to_fnp);
	} else {
		std::cout << "moved file from " << from_fnp.toStdString()
				  << "\n\tto " << to_fnp.toStdString() << "\n";
	}
}
