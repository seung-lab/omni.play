#ifndef OM_SERVER_IMPL_THREAD_HPP
#define OM_SERVER_IMPL_THREAD_HPP

// based off QT FortuneServer example

#include "common/omCommon.h"
#include "network/server/omNetworkQT.hpp"
#include "network/omProcessRequestFromClient.h"

#include <QThread>

class OmServerImplThread : public QThread {
Q_OBJECT

private:
    const int socketDescriptor;
    OmProcessRequestFromClient& processRequest_;

public:
    OmServerImplThread(const int socketDescriptor, QObject* parent,
                       OmProcessRequestFromClient& processRequest)
        : QThread(parent)
        , socketDescriptor(socketDescriptor)
        , processRequest_(processRequest)
    {}

    virtual ~OmServerImplThread();

    void run()
    {
        QTcpSocket socket;

        if (!socket.setSocketDescriptor(socketDescriptor))
        {
            std::cout << "error: " << socket.errorString().toStdString() << "\n";
            return;
        }

        const boost::optional<QString> inStr = om::networkQT::ReadString(&socket);

        QString ret("could not process cmd");
        if(inStr){
            ret = processRequest_.Process(*inStr);
        }

        om::networkQT::WriteString(&socket, ret);

        socket.disconnectFromHost();
        socket.waitForDisconnected();
    }
};

#endif
