#pragma once

#include <boost/scoped_ptr.hpp>

class OmServerImpl;

class OmServer {
private:
    boost::scoped_ptr<OmServerImpl> server_;

public:
    OmServer();
    ~OmServer();

    void Start();
};

