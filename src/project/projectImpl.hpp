#pragma once

/*
 *  Manages data structures that are shared between various parts of the
 *    system.
 *
 *  Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "common/common.h"
#include "datalayer/archive/project.h"
#include "datalayer/fs/fileNames.hpp"
#include "datalayer/dataPaths.h"
#include "datalayer/dataWrapper.h"
#include "project/details/projectVolumes.h"
#include "project/details/segmentationManager.h"
#include "common/genericManager.hpp"
#include "utility/fileHelpers.h"
#include "datalayer/fs/file.h"
#include "datalayer/archive/project.h"
//#include "volume/segmentationDataWrapper.hpp"

#include <fstream>

namespace om {
namespace proj {

class projectImpl {
private:
    std::string projectMetadataFile_;
    std::string filesFolder_;
    std::string omniFile_;

    int fileVersion_;
    bool isReadOnly_;

    volumes volumes_;
    zi::semaphore fileReadThrottle_;

public:
    projectImpl()
        : fileVersion_(0)
        , isReadOnly_(false)
    {
        fileReadThrottle_.set(4);
    }

    ~projectImpl()
    {}

    const std::string& FilesFolder() {
        return filesFolder_;
    }

    const std::string& OmniFile() {
        return omniFile_;
    }

    //volume management
    volumes& Volumes(){
        return volumes_;
    }

    //project IO
    std::string New(const std::string& fileNameAndPathIn)
    {
        const std::string fnp_rel =
            datalayer::fileNames::AddOmniExtensionIfNeeded(fileNameAndPathIn);
        const std::string fnp = file::absolute(fnp_rel);

        doNew(fnp);

        return fnp;
    }

    void Load(const std::string& fileNameAndPath)
    {
        try {
            std::string path = file::absolute(fileNameAndPath);
            doLoad(path);

        } catch(...)
        {
            throw;
        }
    }

    void Save()
    {
//        FOR_EACH(iter, volume::SegmentationDataWrapper::ValidIDs()){
//            SegmentationDataWrapper(*iter).GetSegmentation().Flush();
//        }

        datalayer::archive::project::Write(projectMetadataFile_, this);

//        globals_->Users().UserSettings().Save();

        printf("omni project saved!\n");
    }

    int GetFileVersion() const {
        return fileVersion_;
    }

    bool IsReadOnly() const {
        return isReadOnly_;
    }

    zi::semaphore& FileReadSemaphore(){
        return fileReadThrottle_;
    }


private:

    void doNew(const std::string& fnp)
    {
        omniFile_ = fnp;
        filesFolder_ = fnp + ".files";

        projectMetadataFile_ = datalayer::fileNames::ProjectMetadataFile();

        makeParentFolder();
        doCreate();
        touchEmptyProjectFile();

        Save();
    }

    void makeParentFolder()
    {
        const std::string dirStr = file::absolute(omniFile_);

        if(!file::exists(dirStr))
        {
            utility::fileHelpers::MkDir(dirStr);
        }
    }

    void doLoad(const std::string& fnp)
    {
        if(!file::exists(fnp)){
            throw common::ioException("Project file not found at", fnp);
        }

        omniFile_ = fnp;
        filesFolder_ = fnp + ".files";

        projectMetadataFile_ = datalayer::fileNames::ProjectMetadataFile();

        datalayer::archive::project::Read(projectMetadataFile_, this);

//        globals_->Users().UserSettings().Load();
    }

    void doCreate()
    {
        if(file::exists(omniFile_)) {
            utility::fileHelpers::RmFile(omniFile_);
        }

        utility::fileHelpers::RemoveDir(filesFolder_);
        datalayer::fileNames::MakeFilesFolder();
    }

    void touchEmptyProjectFile()
    {
        std::ofstream file(omniFile_.c_str());
    }

    void setFileVersion(const int v){
        fileVersion_ = v;
    }

    friend class om::project;

    friend YAML::Emitter & YAML::operator<<(YAML::Emitter & out, const projectImpl & p);
    friend void YAML::operator>>(const YAML::Node & in, projectImpl & p);
};

} // namespace proj
} // namespace om
