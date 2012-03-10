#pragma once

#include "common/omCommon.h"

class OmProcessJSONAction;
class OmServiceObjects;

class OmProcessRequestFromClient {
private:
    typedef OmProcessJSONAction* makeActionProcessor();
    std::map<std::string, makeActionProcessor*> actionProcessors_;

public:
    OmProcessRequestFromClient();
    ~OmProcessRequestFromClient();

    QString Process(const QString& cmd);

private:
    boost::scoped_ptr<OmServiceObjects> serviceObjects_;

    std::string doProcess(const QString& cmd);

    OmProcessJSONAction* createActionProcessor(const std::string& action);
};

