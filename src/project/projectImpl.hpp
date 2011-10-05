#pragma once

/*
 *  Manages data structures that are shared between various parts of the
 *    system.
 *
 *  Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "actions/io/omActionLogger.hpp"
#include "actions/io/omActionReplayer.hpp"
#include "actions/omActions.h"
#include "common/om.hpp"
#include "common/common.h"
#include "datalayer/archive/old/dataArchiveProject.h"
#include "datalayer/archive/project.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/dataPath.h"
#include "datalayer/dataPath.h"
#include "datalayer/dataPaths.h"
#include "datalayer/dataWrapper.h"
#include "project/details/projectVolumes.h"
#include "project/details/segmentationManager.h"
#include "project/projectGlobals.h"
#include "system/cache/omCacheManager.h"
#include "system/genericManager.hpp"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omUndoStack.hpp"
#include "tiles/cache/tileCache.h"
#include "users/omGuiUserChooser.h"
#include "utility/omFileHelpers.h"
#include "datalayer/fs/omFile.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "datalayer/archive/project.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

class projectImpl {
private:
    QString projectMetadataFile_;
    QString oldHDF5projectFile_;
    OmHdf5* oldHDF5_;
    QString filesFolder_;
    QString omniFile_;

    int fileVersion_;
    bool isReadOnly_;

    projectVolumes volumes_;
    boost::scoped_ptr<projectGlobals> globals_;

public:
    projectImpl()
        : oldHDF5_(NULL)
        , fileVersion_(0)
        , isReadOnly_(false)
    {}

    ~projectImpl()
    {}

    const QString& FilesFolder() {
        return filesFolder_;
    }

    const QString& OmniFile() {
        return omniFile_;
    }

    bool HasOldHDF5() const {
        return NULL != oldHDF5_;
    }

    OmHdf5* OldHDF5()
    {
        if(!oldHDF5_){
            throw OmIoException("no old hdf5 file present");
        }
        return oldHDF5_;
    }

    //volume management
    projectVolumes& Volumes(){
        return volumes_;
    }

    //project IO
    QString New(const QString& fileNameAndPathIn)
    {
        const QString fnp_rel =
            OmFileNames::AddOmniExtensionIfNeeded(fileNameAndPathIn);
        const QString fnp = QFileInfo(fnp_rel).absoluteFilePath();

        doNew(fnp);

        return fnp;
    }

    void Load(const QString& fileNameAndPath, QWidget* guiParent)
    {
        try {
            const QFileInfo projectFile(fileNameAndPath);
            doLoad(projectFile.absoluteFilePath(), guiParent);

        } catch(...)
        {
            globals_.reset();
            throw;
        }
    }

    void Save()
    {
        FOR_EACH(iter, SegmentationDataWrapper::ValidIDs()){
            SegmentationDataWrapper(*iter).GetSegmentation().Flush();
        }

        om::data::archive::project::Write(projectMetadataFile_, this);
        
        globals_->Users().UserSettings().Save();

        printf("omni project saved!\n");
    }

    int GetFileVersion() const {
        return fileVersion_;
    }

    bool IsReadOnly() const {
        return isReadOnly_;
    }

    projectGlobals& Globals(){
        return *globals_;
    }

private:

    void doNew(const QString& fnp)
    {
        omniFile_ = fnp;
        filesFolder_ = fnp + ".files";

        projectMetadataFile_ = OmFileNames::ProjectMetadataFile();
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
        const QString dirStr = QFileInfo(omniFile_).absolutePath();

        QDir dir(dirStr);

        if(!dir.exists())
        {
            if(!dir.mkpath(dirStr)){
                throw OmIoException("could not make path", dirStr);
            }
        }
    }

    void doLoad(const QString& fnp, QWidget* guiParent)
    {
        if(!QFile::exists(fnp)){
            throw OmIoException("Project file not found at", fnp);
        }

        omniFile_ = fnp;
        filesFolder_ = fnp + ".files";

        
        oldHDF5projectFile_ = OmFileNames::OldHDF5projectFileName();
        projectMetadataFile_ = OmFileNames::ProjectMetadataFile();
        
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
                throw OmIoException("user not choosen");
            }
        }

        OmCacheManager::Reset();
        tileCache::Reset();
        OmActionLogger::Reset();

        
        if (om::file::exists(projectMetadataFile_.toStdString()))
            om::data::archive::project::Read(projectMetadataFile_, this);
        else
            dataArchiveProject::ArchiveRead(OmFileNames::ProjectMetadataFileOld(), this);
        
        globals_->Users().UserSettings().Load();
        
        OmActionReplayer::Replay();
    }

    void doCreate()
    {
        QFile projectFile(omniFile_);
        if(projectFile.exists()){
            projectFile.remove();
        }

        OmFileHelpers::RemoveDir(filesFolder_);
        OmFileNames::MakeFilesFolder();
    }

    void openHDF5()
    {
        if(!QFile::exists(oldHDF5projectFile_)){
            return;
        }

        const bool isReadOnly = true;

        oldHDF5_ = OmHdf5Manager::Get(oldHDF5projectFile_, isReadOnly);
        oldHDF5_->open();
    }

    void touchEmptyProjectFile()
    {
        QFile file(omniFile_);
        if(!file.open(QIODevice::WriteOnly)) {
            throw OmIoException("could not open", omniFile_);
        }
    }

    void migrateFromHdf5()
    {
        OmFileNames::MakeFilesFolder();

        OmFileHelpers::MoveFile(omniFile_, oldHDF5projectFile_);

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

        QFile newProjectMetadafile(OmFileNames::ProjectMetadataFileOld());

        if(!newProjectMetadafile.open(QIODevice::WriteOnly)) {
            throw OmIoException("could not open", projectMetadataFile_);
        }

        newProjectMetadafile.write(data, size);
    }

    void setFileVersion(const int v){
        fileVersion_ = v;
    }

    void setupGlobals()
    {
        globals_.reset(new projectGlobals());
        globals_->Init();
    }

    friend class project;

    friend YAML::Emitter & YAML::operator<<(YAML::Emitter & out, const projectImpl & p);
    friend void YAML::operator>>(const YAML::Node & in, projectImpl & p);
    friend QDataStream &operator<<(QDataStream & out, const projectImpl & p);
    friend QDataStream &operator>>(QDataStream & in, projectImpl & p);
};

