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
#include "common/omCommon.h"
#include "datalayer/archive/omDataArchiveProject.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omSegmentationManager.h"
#include "project/omProjectGlobals.h"
#include "system/cache/omCacheManager.h"
#include "system/omGenericManager.hpp"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omUndoStack.hpp"
#include "tiles/cache/omTileCache.h"
#include "users/omGuiUserChooser.h"
#include "utility/omFileHelpers.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

class OmProjectImpl {
private:
    QString projectMetadataFile_;
    QString oldHDF5projectFile_;
    OmHdf5* oldHDF5_;
    QString filesFolder_;
    QString omniFile_;

    int fileVersion_;
    bool isReadOnly_;

    OmProjectVolumes volumes_;
    boost::scoped_ptr<OmProjectGlobals> globals_;

public:
    OmProjectImpl()
        : oldHDF5_(NULL)
        , fileVersion_(0)
        , isReadOnly_(false)
    {}

    ~OmProjectImpl()
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
    OmProjectVolumes& Volumes(){
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
        const QFileInfo projectFile(fileNameAndPath);
        doLoad(projectFile.absoluteFilePath(), guiParent);
    }

    void Save()
    {
        FOR_EACH(iter, volumes_.Segmentations().GetValidSegmentationIds()){
            volumes_.Segmentations().GetSegmentation(*iter).Flush();
        }

        OmDataArchiveProject::ArchiveWrite(projectMetadataFile_, this);

        printf("omni project saved!\n");
    }

    void Commit()
    {
        Save();
        OmStateManager::UndoStack().Clear();
    }

    int GetFileVersion() const {
        return fileVersion_;
    }

    bool IsReadOnly() const {
        return isReadOnly_;
    }

    OmProjectGlobals& Globals(){
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
        OmTileCache::Reset();

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

        projectMetadataFile_ = OmFileNames::ProjectMetadataFile();
        oldHDF5projectFile_ = OmFileNames::OldHDF5projectFileName();

        migrateFromHdf5();

        setupGlobals();

        OmCacheManager::Reset();
        OmTileCache::Reset();

        if(guiParent)
        {
            OmGuiUserChooser* chooser = new OmGuiUserChooser(guiParent);
            chooser->exec();
        }

        OmDataArchiveProject::ArchiveRead(projectMetadataFile_, this);

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
        if(!QFileInfo(omniFile_).size()){
            oldHDF5projectFile_ = "";
            return;
        }

        OmFileNames::MakeFilesFolder();

        OmFileHelpers::MoveFile(omniFile_, oldHDF5projectFile_);

        touchEmptyProjectFile();

        openHDF5();

        moveProjectMetadata();
    }

    void moveProjectMetadata()
    {
        const OmDataPath path = OmDataPaths::getProjectArchiveNameQT();
        int size;
        OmDataWrapperPtr dw = oldHDF5_->readDataset(path, &size);
        char const*const data = dw->getPtr<const char>();

        QFile newProjectMetadafile(projectMetadataFile_);

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
        globals_.reset(new OmProjectGlobals());
        globals_->Init();
    }

    friend class OmProject;

    friend QDataStream &operator<<(QDataStream & out, const OmProjectImpl & p);
    friend QDataStream &operator>>(QDataStream & in, OmProjectImpl & p);
};

