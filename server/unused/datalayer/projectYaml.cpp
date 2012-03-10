#include "datalayer/archive/project.h"
#include "datalayer/archive/channel.h"
#include "datalayer/archive/segmentation.h"
#include "common/common.h"
#include "utility/fileHelpers.h"
#include "datalayer/fs/file.h"
#include "project/project.h"
#include "project/projectImpl.hpp"
#include "utility/yaml/baseTypes.hpp"
//#include "volume/channelDataWrapper.hpp"
//#include "volume/segmentationDataWrapper.hpp"
//#include "segment/segmentDataWrapper.hpp"
//#include "mesh/meshManagers.hpp"

#include <fstream>
#include <sstream>

namespace om {
namespace datalayer {
namespace archive {

void project::Read(const std::string& fnp, proj::projectImpl* project) {
    using namespace YAML;

    std::ifstream fin(fnp.c_str());

    try
    {
        Parser parser(fin);

        Node doc;
        parser.GetNextDocument(doc);

//        int ver;
//        doc["version"] >> ver;
//        project::setFileVersion(ver);

        parser.GetNextDocument(doc);
        doc >> (*project);
    }
    catch(Exception e)
    {
        std::stringstream ss;
        ss << e.msg << "\n";
        ss << fnp;
        ss << " line: " << e.mark.line;
        ss << " col: " << e.mark.column;
        ss << " pos: " << e.mark.pos;
        throw common::ioException(ss.str());
    }
}

void project::Write(const std::string& fnp, proj::projectImpl* project) {
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
        utility::fileHelpers::MoveFile(fnp, fnpOld);
    } catch(...)
    {}

    std::ofstream file(fnp.c_str());

    file << emitter.c_str();
}

} // namespace archive
} // namespace datalayer
} // namespace om

namespace YAML {

Emitter &operator<<(Emitter& out, const om::proj::projectImpl& p)
{
//    out << BeginMap;
//    out << Key << "Preferences" << Value << OmPreferences::instance();
//    out << Key << "Volumes" << Value << p.volumes_;
//    out << EndMap;
    return out;
}

void operator>>(const Node& in, om::proj::projectImpl& p)
{
//    in["Preferences"] >> OmPreferences::instance();
    in["Volumes"] >> p.volumes_;
}

Emitter &operator<<(Emitter& out, const om::proj::volumes& p)
{
    out << BeginMap;
    out << Key << "Channels" << Value << *p.channels_;
    out << Key << "Segmentations" << Value << *p.segmentations_;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, om::proj::volumes& p)
{
    in["Channels"] >> *p.channels_;
    in["Segmentations"] >> *p.segmentations_;
}

} // namespace YAML
