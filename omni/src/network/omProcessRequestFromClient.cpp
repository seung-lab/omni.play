#include "network/omAssembleTilesIntoSlice.hpp"

#include "common/omString.hpp"
#include "network/omJson.h"
#include "network/omProcessJSONAction.hpp"
#include "network/omProcessRequestFromClient.h"
#include "utility/omStringHelpers.h"

#include <QPainter>

OmProcessRequestFromClient::OmProcessRequestFromClient()
    : tileAssembler_(new OmAssembleTilesIntoSlice())
{
    actionProcessors_["get_projects"] = OmJSONListProjects::CreateInstance;
    actionProcessors_["get_slice"] = OmJSONGetSlice::CreateInstance;
}

OmProcessRequestFromClient::~OmProcessRequestFromClient()
{}

QString OmProcessRequestFromClient::Process(const QString& cmd)
{
    // std::cout << "processing cmd: \"" << cmd.toStdString() << "\"...\n";

    // OmJson json;
    // json.Read(cmd.toStdString());
    // boost::optional<std::string> action = json.GetAction();
    // if(!action){
    //     return "fail";
    // }

    // std::cout << "action: " << *action << "\n";


    // boost::scoped_ptr<OmProcessJSONAction> process(createActionProcessor(*action));
    // if(!process){
    //     return "fail";
    // }

    // return QString::fromStdString(process->Process(json));

    const int sliceNum = OmStringHelpers::getUInt(cmd);
    std::cout << "slice_num: " << sliceNum << "\n";

    return makeImgFiles(sliceNum);
}

OmProcessJSONAction*
OmProcessRequestFromClient::createActionProcessor(const std::string& action)
{
    if(!actionProcessors_.count(action)){
        return NULL;
    }

    return actionProcessors_[action]();
}

/** returns UUID
 *
 * creates
 *   /var/www/temp_omni_imgs/channel-1/UUID.png
 *   /var/www/temp_omni_imgs/segmenation-1/UUID.png
 **/
QString OmProcessRequestFromClient::makeImgFiles(const int sliceNum){
    return tileAssembler_->MakeImgFiles(sliceNum);
}

