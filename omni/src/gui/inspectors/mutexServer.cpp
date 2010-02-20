#include "mutexServer.h"


MutexServer::MutexServer()
{
	mTerminate = false;
}

void MutexServer::safeTerminate ()
{
	mTerminate = true;
}

void MutexServer::run ()
{
	tcpServer = new QTcpServer(this);
	if (!tcpServer->listen( QHostAddress("192.168.3.254"), 8989 )) {
		printf("could not listen!\n");
		return;
	}

	connect(tcpServer, SIGNAL(newConnection()), 
		this, SLOT(dealWithClient()));
}

void MutexServer::dealWithClient()
{
	QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
	connect(clientConnection, SIGNAL(disconnected()),
		clientConnection, SLOT(deleteLater()));
	
	char dummy;
	clientConnection->getChar(&dummy);
	clientConnection->disconnectFromHost();
}
