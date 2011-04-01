#include "network/omProcessRequestFromClient.h"
#include "utility/dataWrappers.h"
#include "common/omString.hpp"
#include "network/omJson.h"
#include "network/omProcessJSONAction.hpp"

OmProcessRequestFromClient::OmProcessRequestFromClient()
{
    actionProcessors_["listProjects"] = OmJSONListProjects::CreateInstance;
}

template <typename T>
void getParamVec(const OmJson& json)
{
    boost::optional<std::vector<T> > params = json.Params<T>();
    if(!params){
        return;
    }

    std::vector<T>& paramsRef = *params;

    FOR_EACH(iter, paramsRef){
        std::cout << "param: " << *iter << "\n";
    }
}

QString OmProcessRequestFromClient::Process(const QString& cmd)
{
    std::cout << "processing cmd: \"" << cmd.toStdString() << "\"...\n";

    OmJson json;
    json.Read(cmd.toStdString());
    boost::optional<std::string> action = json.GetAction();
    if(!action){
        return "fail";
    }

    std::cout << "action: " << *action << "\n";

    try {
        getParamVec<int>(json);
    } catch(...){
    }

    boost::scoped_ptr<OmProcessJSONAction> process(createActionProcessor(*action));
    if(!process){
        return "fail";
    }

    return QString::fromStdString(process->Process(*action));
}

OmProcessJSONAction*
OmProcessRequestFromClient::createActionProcessor(const std::string& action)
{
    if(!actionProcessors_.count(action)){
        return NULL;
    }

    return actionProcessors_[action]();
}
