#pragma once

// based off QT FortuneServer example

#include "network/server/omServerImplThread.hpp"
#include "common/omCommon.h"

#include <QTcpServer>

class OmProcessRequestFromClient;

class OmServerImpl : public QTcpServer {
Q_OBJECT

private:
    OmProcessRequestFromClient processRequest_;

public:
    OmServerImpl(QObject *parent = 0)
        : QTcpServer(parent)
    {
        listen(QHostAddress::Any, 8584);

        const std::string addr = serverAddress().toString().toStdString();

        std::cout << "addr: " << addr
                  << ", port: " << serverPort() << "\n";
    }

    virtual ~OmServerImpl()
    {}

protected:
    void incomingConnection(int socketDescriptor)
    {
        OmServerImplThread *thread =
            new OmServerImplThread(socketDescriptor, this, processRequest_);

        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
    }
};

