#ifndef MUTEX_SERVER_H
#define MUTEX_SERVER_H

#include <QThread>
#include <QMutex>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class MutexServer : public QThread
{
	Q_OBJECT
public:
	MutexServer(QString, int);

	void safeTerminate ();
        void run();
private:
	QTcpServer* tcpServer;
	bool mTerminate;

	QString mHost;
	int mPort;

	QMutex * fileLock;
};

#endif
