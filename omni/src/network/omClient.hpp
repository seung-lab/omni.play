#ifndef OM_CLIENT_HPP
#define OM_CLIENT_HPP

#include "common/omCommon.h"
#include "network/omNetworkQT.hpp"

class OmClient : public QTcpSocket {
Q_OBJECT

public:
    OmClient()
    {}

    void Send(const std::string& cmd)
    {
        connectToHost(QHostAddress::Any, 8584);

        if(!waitForConnected(om::networkQT::timeoutMS)){
            std::cout << "failed to connect\n";
            return;
        }

        om::networkQT::WriteString(this, QString::fromStdString(cmd));
        const boost::optional<QString> inStr = om::networkQT::ReadString(this);

        if(inStr){
            std::cout << inStr->toStdString() << "\n";
        }
    }
};

#endif
