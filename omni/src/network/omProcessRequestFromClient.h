#ifndef OM_PROCESS_REQUEST_FROM_CLIENT_HPP
#define OM_PROCESS_REQUEST_FROM_CLIENT_HPP

#include "common/omCommon.h"

class OmProcessJSONAction;

class OmProcessRequestFromClient {
private:
    typedef OmProcessJSONAction* makeActionProcessor();
    std::map<std::string, makeActionProcessor*> actionProcessors_;

public:
    OmProcessRequestFromClient();

    QString Process(const QString& cmd);

private:
    OmProcessJSONAction* createActionProcessor(const std::string& action);
};

#endif
