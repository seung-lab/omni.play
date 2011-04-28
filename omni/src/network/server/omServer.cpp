#include "network/server/omServer.h"
#include "network/server/omServerImpl.hpp"
#include "common/omCommon.h"

OmServer::OmServer()
{}

OmServer::~OmServer()
{}

void OmServer::Start()
{
    server_.reset(new OmServerImpl());
    std::cout << "server started\n";
}
