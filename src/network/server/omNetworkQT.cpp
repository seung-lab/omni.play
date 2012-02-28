#include "network/server/omNetworkQT.hpp"

boost::optional<QString> om::networkQT::ReadString(QTcpSocket* socket)
{
    if(!socket->waitForReadyRead(timeoutMS))
    {
        std::cout << "socket wait for full read error: "
                  << socket->errorString().toStdString() << "\n";
        return boost::optional<QString>();
    }

    return QString(socket->readAll());
}

void om::networkQT::WriteString(QTcpSocket* socket, const QString& str)
{
    socket->write(qPrintable(str), str.size());
}
