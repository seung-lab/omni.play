#include "mutexServer.h"
#include <QCoreApplication>


MutexServerWorkerThread::MutexServerWorkerThread (QTcpSocket *clientConnection, QSemaphore * fileLock)
{
	mClientConnection = clientConnection;
	mFileLock = fileLock;
	connect(clientConnection, SIGNAL(readyRead()), this, SLOT(handleRequest()));
	connect(clientConnection, SIGNAL(disconnected()),
		this, SLOT(releaseLock()));
}
void MutexServerWorkerThread::releaseLock()
{
	printf ("releasing lock...\n");
	mFileLock->release(1);
	mClientConnection->deleteLater();
	quit ();
}
void MutexServerWorkerThread::handleRequest ()
{
	char dummy;
	mClientConnection->getChar(&dummy);
	printf ("got request...\n");
	if ('L' == dummy) {
		printf ("getting lock...\n");
		mFileLock->acquire(1);
		printf ("lock was assigned\n");
		char lock = 'l';
		mClientConnection->putChar(lock);
		mClientConnection->flush ();
	}
}

MutexServer::MutexServer(QString host, int port)
{
	mTerminate = false;
	mHost = host;
	mPort = port;

	fileLock = new QSemaphore (1);
}

void MutexServer::safeTerminate ()
{
	mTerminate = true;
}

void MutexServer::run ()
{
	tcpServer = new QTcpServer(this);

	QObject::connect(tcpServer, SIGNAL(newConnection()), 
		this, SLOT(dealWithClient()));

	//if (!tcpServer->listen( QHostAddress(mHost), mPort)) {
	if (!tcpServer->listen( QHostAddress::LocalHost, mPort)) {
		printf("could not listen!\n");
		return;
	}
}

void MutexServer::dealWithClient()
{
	
	printf ("getting connection request...\n");
	QTcpSocket *clientConnection = tcpServer->nextPendingConnection();

	(new MutexServerWorkerThread (clientConnection, fileLock))->start();
}

