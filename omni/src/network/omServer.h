#ifndef OM_SERVER_HPP
#define OM_SERVER_HPP

#include <boost/shared_ptr.hpp>

class OmServerImpl;

class OmServer {
private:
    boost::shared_ptr<OmServerImpl> server_;

public:
    OmServer()
    {}

    virtual ~OmServer();

    void Start();
};

#endif
