#include "common/debug.h"
#include "utility/fileHelpers.h"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

using namespace boost::filesystem;

namespace om {
namespace utility {
        
bool fileHelpers::IsFileReadOnly(const std::string& fileNameAndPath)
{
    file_status s = status(fileNameAndPath);
    
    if (exists(s)){
        return true;
    }

    return false;
}

void fileHelpers::RemoveDir(const std::string &folder)
{
    if(exists(status(folder)))
    {
        std::cout << "removing folder " << folder << "... ";
        if(remove_all(folder)){
            std::cout << "done!\n";
        } else {
            throw IoException("could not remove folder " + folder);
        }
    }
}

void fileHelpers::MoveFile(const std::string& from_fnp, const std::string& to_fnp)
{
    if(!exists(from_fnp)){
        throw IoException("could not find file " + from_fnp);
    }

    if(exists(to_fnp))
    {
        if(!remove_all(to_fnp)){
            throw IoException("could not remove previous file " + to_fnp);
        }
    }
    
    rename(from_fnp, to_fnp);
    std::cout << "moved file from " << from_fnp
              << "\n\tto " << to_fnp << "\n";
}

void fileHelpers::CopyFile(const std::string& from_fnp, const std::string& to_fnp)
{
    copy_file(from_fnp, to_fnp);
}

bool fileHelpers::IsFolderEmpty(const std::string& dirNameQT)
{
    const std::string dirName = dirNameQT;

    if(!exists(dirName)){
        return true;
    }

    return directory_iterator(dirName) == directory_iterator();
}

void fileHelpers::MoveAllFiles(const std::string& fromDir, const std::string& toDir)
{
    path from(fromDir);

    if(!exists(from) || !is_directory(from)){
        throw IoException("invalid folder name " + fromDir);
    }

    path to(toDir);

    if(!exists(to) || !is_directory(to)){
        throw IoException("invalid folder name " + toDir);
    }

    directory_iterator iter(from);
    directory_iterator dir_end;

    for( ; iter != dir_end; ++iter)
    {
        path path = *iter;

        std::cout << "moving file/folder: " << path.string() << "\n";
        rename(path, toDir);
    }
}

bool fileHelpers::MkDir(const std::string& dirName)
{
    path path(dirName);

    try{
        return create_directories(path);

    } catch(...){
        throw IoException("could not create directory " + dirName);
    }
}

bool fileHelpers::IsFolder(const std::string& dirName)
{
    path path(dirName);

    return is_directory(path);
}

void fileHelpers::Symlink(const std::string& fromDir, const std::string& toDir)
{
    create_symlink(fromDir, toDir);
}

bool fileHelpers::IsSymlink(const std::string& fileName){
    return is_symlink(fileName);
}

void fileHelpers::RmFile(const std::string& fileName)
{
    remove(fileName);
}

} // namespace utility
} // namespace om