#include "common/common.h"
#include "datalayer/archive/old/omDataArchiveProject.h"
#include "datalayer/archive/old/omDataArchiveProjectImpl.h"
#include "datalayer/upgraders/omUpgraders.hpp"
#include "mesh/omMeshManagers.hpp"
#include "project/omProject.h"
#include "utility/dataWrappers.h"
#include "utility/omFileHelpers.h"

#include <QFile>

static const int Latest_Project_Version = 25;
static const QString Omni_Postfix("OMNI");
static int fileVersion_;

void OmDataArchiveProject::ArchiveRead(const QString& fnp, OmProjectImpl* project)
{
    QFile file(fnp);

    om::file::old::openFileRO(file);

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_4_6);

    in >> fileVersion_;
    OmProject::setFileVersion(fileVersion_);
    printf("Omni file version is %d\n", fileVersion_);

    if(fileVersion_ < 10 || fileVersion_ > Latest_Project_Version)
    {
        const QString err =
            QString("can not open file: file version is (%1), but Omni expecting (%2)")
            .arg(fileVersion_)
            .arg(Latest_Project_Version);
        throw om::IoException(err.toStdString());
    }

    in >> (*project);

    QString omniPostfix;
    in >> omniPostfix;

    if(Omni_Postfix != omniPostfix || !in.atEnd()){
        throw om::IoException("corruption detected in Omni file");
    }

    if(fileVersion_ < Latest_Project_Version)
    {
        upgrade();
        //ArchiveWrite(fnp, project);
    }

    postLoad();
}

void OmDataArchiveProject::postLoad()
{
    FOR_EACH(iter, ChannelDataWrapper::ValidIDs())
    {
        const ChannelDataWrapper cdw(*iter);

        if(cdw.IsBuilt())
        {
            std::vector<OmFilter2d*> filters = cdw.GetFilters();

            FOR_EACH(fiter, filters)
            {
                OmFilter2d* filter = *fiter;
                filter->Load();
            }
        }
    }

    FOR_EACH(iter, SegmentationDataWrapper::ValidIDs())
    {
        const SegmentationDataWrapper sdw(*iter);
        if(sdw.IsBuilt()){
            sdw.GetSegmentation().MeshManagers()->Load();
        }
    }
}

void OmDataArchiveProject::upgrade()
{
    if(fileVersion_ < 14)
    {
        OmUpgraders::to14();
        OmUpgraders::to20();
        OmUpgraders::RebuildCenterOfSegmentData();

    } else if(fileVersion_ < 19)
    {
        OmUpgraders::to20();
        OmUpgraders::RebuildCenterOfSegmentData();

    } else if(fileVersion_ < 20){
        OmUpgraders::to20();
    }
}
