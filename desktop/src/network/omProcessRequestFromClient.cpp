#include "common/omString.hpp"
#include "network/json/omJson.h"
#include "network/omJSONActions.hpp"
#include "network/omProcessRequestFromClient.h"
#include "utility/omDataTime.hpp"
#include "utility/omStringHelpers.h"

#include <QPainter>

OmProcessRequestFromClient::OmProcessRequestFromClient()
    : serviceObjects_(new OmServiceObjects)
{
    actionProcessors_["change_threshold"] = OmJSONChangeThreshold::CreateInstance;
    actionProcessors_["get_mesh_data"] = OmJSONGetMeshData::CreateInstance;
    actionProcessors_["get_project_info"] = OmJSONProjectInfo::CreateInstance;
    actionProcessors_["get_segmentation_dim"] = OmJSONSegmentationDim::CreateInstance;
    actionProcessors_["get_slice_channel"] = OmJSONGetSliceChannel::CreateInstance;
    actionProcessors_["get_slice_segmentation"] = OmJSONGetSliceSegmentation::CreateInstance;
    actionProcessors_["get_tile_channel"] = OmJSONGetTileChannel::CreateInstance;
    actionProcessors_["get_tile_segmentation"] = OmJSONGetTileSegmentation::CreateInstance;
    actionProcessors_["select_segment"] = OmJSONSelectSegment::CreateInstance;
}

OmProcessRequestFromClient::~OmProcessRequestFromClient()
{}

QString OmProcessRequestFromClient::Process(const QString& cmd){
    return QString::fromStdString(doProcess(cmd));
}

std::string OmProcessRequestFromClient::doProcess(const QString& cmd)
{
     std::cout << "processing cmd: \"" << cmd.toStdString() << "\"..."
               << "(" << om::datetime::cur() << ")\n";

     OmJson json;
     json.Read(cmd.toStdString());
     boost::optional<std::string> action = json.GetAction();
     if(!action){
         return "fail";
     }

     boost::scoped_ptr<OmProcessJSONAction> process(createActionProcessor(*action));
     if(!process){
         return "could not create processor for " + *action;
     }

     return process->Process(json, serviceObjects_.get());
}

OmProcessJSONAction*
OmProcessRequestFromClient::createActionProcessor(const std::string& action)
{
    if(!actionProcessors_.count(action)){
        return NULL;
    }

    return actionProcessors_[action]();
}

