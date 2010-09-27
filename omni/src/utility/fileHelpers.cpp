#include "common/omDebug.h"
#include "utility/fileHelpers.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

bool FileHelpers::isFileReadOnly(const std::string& fileNameAndPath)
{
	QFileInfo file(QString::fromStdString(fileNameAndPath));
	if (file.exists() && !file.isWritable() ){
		return true;
	}

	return false;
 }


/*!
   Delete a directory along with all of its contents.

   \param dirName Path of directory to remove.
   \return true on success; false on error.

   from http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
*/
bool FileHelpers::removeDir(const QString &dirName)
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
