#include "network/omServer.h"
#include "network/omServerImpl.hpp"
#include "common/omCommon.h"

OmServer::~OmServer()
{}

void OmServer::Start()
{
    server_.reset(new OmServerImpl());
    std::cout << "server started\n";
}
