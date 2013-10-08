#include "common/omDebug.h"
#include "utility/omFileHelpers.h"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

bool OmFileHelpers::IsFileReadOnly(const std::string& fileNameAndPath) {
  QFileInfo file(QString::fromStdString(fileNameAndPath));

  if (file.exists() && !file.isWritable()) {
    return true;
  }

  return false;
}

void OmFileHelpers::RemoveDir(const QString& folder) {
  QDir filesDir(folder);
  if (filesDir.exists()) {
    printf("removing folder %s...", qPrintable(folder));
    fflush(stdout);
    if (removeDir(folder)) {
      printf("done!\n");
    } else {
      throw OmIoException("could not remove folder", folder);
    }
  }
}

bool OmFileHelpers::removeDir(const QString& dirNameQT) {
  boost::filesystem::path dir(dirNameQT.toStdString());

  return boost::filesystem::remove_all(dir);
}

void OmFileHelpers::MoveFile(const QString& from_fnp, const QString& to_fnp) {
  QFile file(from_fnp);
  if (!file.exists()) {
    throw OmIoException("could not find file", from_fnp);
  }

  if (QFile::exists(to_fnp)) {
    if (!QFile::remove(to_fnp)) {
      throw OmIoException("could not remove previous file", to_fnp);
    }
  }

  if (!file.rename(to_fnp)) {
    throw OmIoException("could not rename file to", to_fnp);

  } else {
    std::cout << "moved file from " << from_fnp.toStdString() << "\n\tto "
              << to_fnp.toStdString() << "\n";
  }
}

void OmFileHelpers::CopyFile(const QString& from_fnp, const QString& to_fnp) {
  QFile file(from_fnp);
  if (!file.exists()) {
    throw OmIoException("could not find file", from_fnp);
  }

  if (QFile::exists(to_fnp)) {
    if (!QFile::remove(to_fnp)) {
      throw OmIoException("could not remove previous file", to_fnp);
    }
  }

  if (!QFile::copy(from_fnp, to_fnp)) {
    throw OmIoException("could not copy to file", to_fnp);
  }
}

bool OmFileHelpers::IsFolderEmpty(const QString& dirNameQT) {
  const std::string dirName = dirNameQT.toStdString();

  if (!boost::filesystem::exists(dirName)) {
    return true;
  }

  return boost::filesystem::directory_iterator(dirName) ==
         boost::filesystem::directory_iterator();
}

void OmFileHelpers::MoveAllFiles(const QString& fromDirQT,
                                 const QString& toDirQT) {
  boost::filesystem::path fromDir(fromDirQT.toStdString());

  if (!boost::filesystem::exists(fromDir) ||
      !boost::filesystem::is_directory(fromDir)) {
    throw OmIoException("invalid folder name", fromDirQT);
  }

  boost::filesystem::path toDir(toDirQT.toStdString());

  if (!boost::filesystem::exists(toDir) ||
      !boost::filesystem::is_directory(toDir)) {
    throw OmIoException("invalid folder name", toDirQT);
  }

  boost::filesystem::directory_iterator iter(fromDir);
  boost::filesystem::directory_iterator dir_end;

  for (; iter != dir_end; ++iter) {
    boost::filesystem::path path = *iter;

    std::cout << "moving file/folder: " << path.string() << "\n";
    boost::filesystem::rename(path, toDir);
  }
}

bool OmFileHelpers::MkDir(const QString& dirNameQT) {
  return MkDir(dirNameQT.toStdString());
}

bool OmFileHelpers::MkDir(const std::string& dirName) {
  boost::filesystem::path path(dirName);

  try {
    return boost::filesystem::create_directories(path);

  }
  catch (...) {
    throw OmIoException("could not create directory", dirName);
  }
}

bool OmFileHelpers::IsFolder(const QString& dirNameQT) {
  boost::filesystem::path path(dirNameQT.toStdString());

  return boost::filesystem::is_directory(path);
}

void OmFileHelpers::Symlink(const QString& fromDirQT, const QString& toDirQT) {
  boost::filesystem::path fromDir(fromDirQT.toStdString());
  boost::filesystem::path toDir(toDirQT.toStdString());

  boost::filesystem::create_symlink(fromDir, toDir);
}

bool OmFileHelpers::IsSymlink(const QString& fileNameQT) {
  return QFileInfo(fileNameQT).isSymLink();
}

void OmFileHelpers::RmFile(const QString& fileNameQT) {
  boost::filesystem::path path(fileNameQT.toStdString());

  boost::filesystem::remove(path);
}

bool OmFileHelpers::DoesFileExist(const QString& fnp) {
  return QFileInfo(fnp).exists();
}
