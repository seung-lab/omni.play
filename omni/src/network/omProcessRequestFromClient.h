#ifndef OM_PROCESS_REQUEST_FROM_CLIENT_HPP
#define OM_PROCESS_REQUEST_FROM_CLIENT_HPP

#include "common/omCommon.h"

class OmProcessJSONAction;
class OmAssembleTilesIntoSlice;

class OmProcessRequestFromClient {
private:
    typedef OmProcessJSONAction* makeActionProcessor();
    std::map<std::string, makeActionProcessor*> actionProcessors_;

public:
    OmProcessRequestFromClient();
    ~OmProcessRequestFromClient();

    QString Process(const QString& cmd);

private:
    boost::scoped_ptr<OmAssembleTilesIntoSlice> tileAssembler_;

    OmProcessJSONAction* createActionProcessor(const std::string& action);
    QString makeImgFiles(const int sliceNum);
};

#endif
