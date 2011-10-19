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
        FOR_EACH(iter, SegmentationDataWrapper::ValidIDs()){
            SegmentationDataWrapper(*iter).GetSegmentation().Flush();
        }

        datalayer::archive::project::Write(projectMetadataFile_, this);

        globals_->Users().UserSettings().Save();

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

        projectMetadataFile_ = fileNames::ProjectMetadataFile();
        oldHDF5projectFile_ = "";

        makeParentFolder();
        doCreate();
        touchEmptyProjectFile();

        setupGlobals();

        OmCacheManager::Reset();
        tileCache::Reset();

        OmPreferenceDefaults::SetDefaultAllPreferences();

        Save();
    }

    void makeParentFolder()
    {
        const std::string dirStr = QFileInfo(omniFile_).absolutePath();

        QDir dir(dirStr);

        if(!dir.exists())
        {
            if(!dir.mkpath(dirStr)){
                throw common::ioException("could not make path", dirStr);
            }
        }
    }

    void doLoad(const std::string& fnp)
    {
        if(!file::exists(fnp)){
            throw common::ioException("Project file not found at", fnp);
        }

        omniFile_ = fnp;
        filesFolder_ = fnp + ".files";


        oldHDF5projectFile_ = fileNames::OldHDF5projectFileName();
        projectMetadataFile_ = fileNames::ProjectMetadataFile();

        if(!QFileInfo(omniFile_).size())
            oldHDF5projectFile_ = "";
        else
            migrateFromHdf5();

        setupGlobals();

        if(guiParent)
        {
            OmGuiUserChooser* chooser = new OmGuiUserChooser(guiParent);
            const int userWasSelected = chooser->exec();

            if(!userWasSelected){
                throw common::ioException("user not choosen");
            }
        }

        OmCacheManager::Reset();
        tileCache::Reset();
        OmActionLogger::Reset();


        if (om::file::exists(projectMetadataFile_.toStdString()))
            datalayer::archive::project::Read(projectMetadataFile_, this);
        else
            dataArchiveProject::ArchiveRead(fileNames::ProjectMetadataFileOld(), this);

        globals_->Users().UserSettings().Load();

        OmActionReplayer::Replay();
    }

    void doCreate()
    {
        QFile projectFile(omniFile_);
        if(projectFile.exists()){
            projectFile.remove();
        }

        fileHelpers::RemoveDir(filesFolder_);
        fileNames::MakeFilesFolder();
    }

    void touchEmptyProjectFile()
    {
        QFile file(omniFile_);
        if(!file.open(QIODevice::WriteOnly)) {
            throw common::ioException("could not open", omniFile_);
        }
    }

    void migrateFromHdf5()
    {
        fileNames::MakeFilesFolder();

        fileHelpers::MoveFile(omniFile_, oldHDF5projectFile_);

        touchEmptyProjectFile();

        openHDF5();

        moveProjectMetadata();

        // TODO: Save()?
    }

    void moveProjectMetadata()
    {
        const dataPath path = dataPaths::getProjectArchiveNameQT();
        int size;
        dataWrapperPtr dw = oldHDF5_->readDataset(path, &size);
        char const*const data = dw->getPtr<const char>();

        QFile newProjectMetadafile(fileNames::ProjectMetadataFileOld());

        if(!newProjectMetadafile.open(QIODevice::WriteOnly)) {
            throw common::ioException("could not open", projectMetadataFile_);
        }

        newProjectMetadafile.write(data, size);
    }

    void setFileVersion(const int v){
        fileVersion_ = v;
    }

    friend class project;

    friend YAML::Emitter & YAML::operator<<(YAML::Emitter & out, const projectImpl & p);
    friend void YAML::operator>>(const YAML::Node & in, projectImpl & p);
};

} // namespace proj
} // namespace om
