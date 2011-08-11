#include "datalayer/archive/project.h"
#include "datalayer/archive/channel.h"
#include "datalayer/archive/segmentation.h"
#include "common/omCommon.h"
#include "utility/omFileHelpers.h"
#include "datalayer/fs/omFile.hpp"
#include "project/omProject.h"
#include "system/omPreferences.h"
#include "project/omProjectImpl.hpp"
#include "datalayer/archive/baseTypes.hpp"
#include "utility/channelDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/segmentDataWrapper.hpp"
#include "volume/omFilter2d.h"
#include "mesh/omMeshManagers.hpp"

#include <qtextstream.h>
#include <fstream>
#include <sstream>

namespace om {
namespace data {
namespace archive {
    
void project::Read(const QString& fnp, OmProjectImpl* project) {
    std::ifstream fin(fnp.toStdString().c_str());
    
    try
    {
        YAML::Parser parser(fin);
        
        YAML::Node doc;
        parser.GetNextDocument(doc);
        
        int ver;
        doc["version"] >> ver;
        OmProject::setFileVersion(ver);
        
        parser.GetNextDocument(doc);
        doc >> (*project);
    }
    catch(YAML::Exception e)
    {
        std::stringstream ss;
        ss << e.msg << "\n"; 
        ss << fnp.toStdString();
        ss << " line: " << e.mark.line;
        ss << " col: " << e.mark.column;
        ss << " pos: " << e.mark.pos;
        throw OmIoException(ss.str());
    }
    postLoad();
}

void project::Write(const QString& fnp, OmProjectImpl* project) {
    YAML::Emitter emitter;
    
    emitter << YAML::BeginDoc << YAML::BeginMap;
    emitter << YAML::Key << "version" << YAML::Value << Latest_Project_Version;
    emitter << YAML::EndMap << YAML::EndDoc;
    
    emitter << YAML::BeginDoc;
    emitter << *project;
    emitter << YAML::EndDoc;
    
    const QString fnpOld = fnp + ".old";
    
    try {
        OmFileHelpers::MoveFile(fnp, fnpOld);
    } catch(...)
    {}
    
    QFile file(fnp);
    
    om::file::openFileWO(file);
    
    QTextStream out(&file);
    
    OmProject::setFileVersion(Latest_Project_Version);
    
    out << emitter.c_str();
}

void project::postLoad()
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

YAML::Emitter &operator<<(YAML::Emitter& out, const OmProjectImpl& p)
{
    out << YAML::BeginMap;
    out << YAML::Key << "Preferences" << YAML::Value << OmPreferences::instance();
    out << YAML::Key << "Volumes" << YAML::Value << p.volumes_;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmProjectImpl& p)
{
    in["Preferences"] >> OmPreferences::instance();
    in["Volumes"] >> p.volumes_;
}

YAML::Emitter &operator<<(YAML::Emitter& out, const OmPreferences& p)
{
    out << YAML::BeginMap;
    out << YAML::Key << "String Preferences" << YAML::Value << p.stringPrefs_;
    out << YAML::Key << "Float Preferences" << YAML::Value << p.floatPrefs_;
    out << YAML::Key << "Int Preferences" << YAML::Value << p.intPrefs_;
    out << YAML::Key << "Bool Preferences" << YAML::Value << p.boolPrefs_;
    out << YAML::Key << "V3f Preferences" << YAML::Value << p.v3fPrefs_;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmPreferences& p)
{
    in["String Preferences"] >> p.stringPrefs_;
    in["Float Preferences"] >> p.floatPrefs_;
    in["Int Preferences"] >> p.intPrefs_;
    in["Bool Preferences"] >> p.boolPrefs_;
    in["V3f Preferences"] >> p.v3fPrefs_;
}

YAML::Emitter &operator<<(YAML::Emitter& out, const OmProjectVolumes& p)
{
    out << YAML::BeginMap;
    out << YAML::Key << "Channels" << YAML::Value << *p.channels_;
    out << YAML::Key << "Segmentations" << YAML::Value << *p.segmentations_;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmProjectVolumes& p)
{
    in["Channels"] >> *p.channels_;
    in["Segmentations"] >> *p.segmentations_;
}


}; // namespace archive
}; // namespace data
}; // namespace om