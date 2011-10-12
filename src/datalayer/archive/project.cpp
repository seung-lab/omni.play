#include "datalayer/archive/project.h"
#include "datalayer/archive/channel.h"
#include "datalayer/archive/segmentation.h"
#include "common/common.h"
#include "utility/omFileHelpers.h"
#include "datalayer/fs/file.h"
#include "project/project.h"
#include "system/omPreferences.h"
#include "project/projectImpl.hpp"
#include "utility/yaml/baseTypes.hpp"
#include "utility/channelDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/segmentDataWrapper.hpp"
#include "volume/omFilter2d.h"
#include "mesh/mesh::managers.hpp"

#include <qtextstream.h>
#include <fstream>
#include <sstream>

namespace om {
namespace datalayer {
namespace archive {
    
void project::Read(const std::string& fnp, projectImpl* project) {
    using namespace YAML;
    
    std::ifstream fin(fnp.toStdString().c_str());
    
    try
    {
        Parser parser(fin);
        
        Node doc;
        parser.GetNextDocument(doc);
        
        int ver;
        doc["version"] >> ver;
        project::setFileVersion(ver);
        
        parser.GetNextDocument(doc);
        doc >> (*project);
    }
    catch(Exception e)
    {
        std::stringstream ss;
        ss << e.msg << "\n"; 
        ss << fnp.toStdString();
        ss << " line: " << e.mark.line;
        ss << " col: " << e.mark.column;
        ss << " pos: " << e.mark.pos;
        throw common::ioException(ss.str());
    }
    postLoad();
}

void project::Write(const std::string& fnp, projectImpl* project) {
    using namespace YAML;
    
    Emitter emitter;
    
    emitter << BeginDoc << BeginMap;
    emitter << Key << "version" << Value << Latest_Project_Version;
    emitter << EndMap << EndDoc;
    
    emitter << BeginDoc;
    emitter << *project;
    emitter << EndDoc;
    
    const std::string fnpOld = fnp + ".old";
    
    try {
        fileHelpers::MoveFile(fnp, fnpOld);
    } catch(...)
    {}
    
    QFile file(fnp);
    
    om::file::openFileWO(file);
    
    QTextStream out(&file);
    
    project::setFileVersion(Latest_Project_Version);
    
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

} // namespace archive
} // namespace datalayer
} // namespace om

namespace YAML {
    
Emitter &operator<<(Emitter& out, const projectImpl& p)
{
    out << BeginMap;
    out << Key << "Preferences" << Value << OmPreferences::instance();
    out << Key << "Volumes" << Value << p.volumes_;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, projectImpl& p)
{
    in["Preferences"] >> OmPreferences::instance();
    in["Volumes"] >> p.volumes_;
}

Emitter &operator<<(Emitter& out, const OmPreferences& p)
{
    out << BeginMap;
    out << Key << "String Preferences" << Value << p.stringPrefs_;
    out << Key << "Float Preferences" << Value << p.floatPrefs_;
    out << Key << "Int Preferences" << Value << p.intPrefs_;
    out << Key << "Bool Preferences" << Value << p.boolPrefs_;
    out << Key << "V3f Preferences" << Value << p.v3fPrefs_;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmPreferences& p)
{
    in["String Preferences"] >> p.stringPrefs_;
    in["Float Preferences"] >> p.floatPrefs_;
    in["Int Preferences"] >> p.intPrefs_;
    in["Bool Preferences"] >> p.boolPrefs_;
    in["V3f Preferences"] >> p.v3fPrefs_;
}

Emitter &operator<<(Emitter& out, const projectVolumes& p)
{
    out << BeginMap;
    out << Key << "Channels" << Value << *p.channels_;
    out << Key << "Segmentations" << Value << *p.segmentations_;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, projectVolumes& p)
{
    in["Channels"] >> *p.channels_;
    in["Segmentations"] >> *p.segmentations_;
}

} // namespace YAML
