#include "datalayer/file.h"

#include <boost/filesystem.hpp>

#include <iostream>

namespace boost {
namespace filesystem {
const directory_iterator& begin(const directory_iterator& iter) { return iter; }
directory_iterator end(const directory_iterator&) {
  return directory_iterator();
}
}
}

namespace om {
namespace file {

using namespace boost::filesystem;

uint64_t numBytes(const path& fnp) {
  if (!exists(fnp)) {
    throw IoException("file not found", fnp.string());
  }
  return boost::filesystem::file_size(fnp);
}

void resizeFileNumBytes(const path& file, const int64_t numBytes) {
  boost::system::error_code ec;
  if (!exists(file)) {
    std::ofstream creator(file.c_str());
  }

  boost::filesystem::resize_file(file, numBytes, ec);
  if (ec) {
    throw IoException(
        str(boost::format("could not resize file %1% to %2% bytes. %3%") %
            file % numBytes % ec.message()));
  }
}

path tempPath() { return boost::filesystem::temp_directory_path(); }
path tempFile() { return tempPath() / boost::filesystem::unique_path(); }

void RemoveDir(const path& folder) {
  if (exists(status(folder))) {
    log_debugs << "removing folder " << folder << "... ";
    if (remove_all(folder)) {
      log_debugs << "done!\n";
    } else {
      throw IoException("could not remove folder", folder.string());
    }
  }
}

void MoveFile(const path& from_fnp, const path& to_fnp) {
  if (!exists(from_fnp)) {
    throw IoException("could not find file", from_fnp.string());
  }

  if (exists(to_fnp)) {
    if (!remove_all(to_fnp)) {
      throw IoException("could not remove previous file", to_fnp.string());
    }
  }

  rename(from_fnp, to_fnp);
  log_debugs << "moved file from " << from_fnp.string() << "\tto "
             << to_fnp.string();
}

void CopyFile(const path& from_fnp, const path& to_fnp, bool overwrite) {
  if (overwrite) {
    copy_file(from_fnp, to_fnp, copy_option::overwrite_if_exists);
  } else {
    copy_file(from_fnp, to_fnp);
  }
}

bool IsFolderEmpty(const path& dirName) {
  if (!exists(dirName)) {
    return true;
  }

  return directory_iterator(dirName) == directory_iterator();
}

void MoveAllFiles(const path& fromDir, const path& toDir) {
  path from(fromDir);

  if (!exists(from) || !is_directory(from)) {
    throw IoException("invalid folder name", fromDir.string());
  }

  path to(toDir);

  if (!exists(to) || !is_directory(to)) {
    throw IoException("invalid folder name", toDir.string());
  }

  directory_iterator iter(from);
  directory_iterator dir_end;

  for (; iter != dir_end; ++iter) {
    path path = *iter;

    log_debugs << "moving file/folder: " << path.string();
    rename(path, toDir);
  }
}

bool MkDir(const path& dirName) {
  path path(dirName);

  try {
    return create_directories(path);
  }
  catch (...) {
    throw IoException("could not create directory", dirName.string());
  }
}

void CreateFolder(const path& fullPath, zi::rwmutex& lock) {
  if (exists(fullPath)) {
    return;
  }

  {
    zi::rwmutex::write_guard g(lock);

    if (!exists(fullPath)) {
      MkDir(fullPath);
    }
  }
}

bool IsFolder(const path& dirName) {
  path path(dirName);

  return is_directory(path);
}

void Symlink(const path& fromDir, const path& toDir) {
  create_symlink(fromDir, toDir);
}

bool IsSymlink(const path& fileName) { return is_symlink(fileName); }

void RmFile(const path& fileName) { remove(fileName); }

}  // namespace file
}  // namespace om
