#include "network/omNetworkQT.hpp"

boost::optional<QString> om::networkQT::ReadString(QTcpSocket* socket)
{
    while(socket->bytesAvailable() < (int)sizeof(quint16)) {
        if(!socket->waitForReadyRead(timeoutMS)){
            std::cout << "socket wait for full read error: "
                      << socket->errorString().toStdString() << "\n";
            return boost::optional<QString>();
        }
    }

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    quint16 blockSize = 0;
    in >> blockSize;

    while (socket->bytesAvailable() < blockSize) {
        if (!socket->waitForReadyRead(timeoutMS)) {
            std::cout << "socket wait for full read error: "
                      << socket->errorString().toStdString() << "\n";
            return boost::optional<QString>();
        }
    }

    QString str;
    in >> str;

    return boost::optional<QString>(str);
}

void om::networkQT::WriteString(QTcpSocket* socket, const QString& str)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << str;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);
}
