#ifndef MUTEX_SERVER_H
#define MUTEX_SERVER_H

#include <qthread.h>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class MutexServer: public QThread
{
public:
	MutexServer();

	void safeTerminate ();
        virtual void run();

private:
	QTcpServer* tcpServer;
	bool mTerminate;
	
	void dealWithClient();
};

#endif
