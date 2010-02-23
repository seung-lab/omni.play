#ifndef MUTEX_SERVER_H
#define MUTEX_SERVER_H

#include <QThread>
#include <QSemaphore>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class MutexServerWorkerThread : public QThread
{
        Q_OBJECT
public:
        MutexServerWorkerThread (QTcpSocket *clientConnection, QSemaphore * fileLock);
public slots:
        void releaseLock ();
        void handleRequest ();
private:
        QSemaphore * mFileLock;
        QTcpSocket * mClientConnection;
};


class MutexServer : public QObject
{
	Q_OBJECT
public:
	MutexServer(QString, int);

	void safeTerminate ();
        void run();
	void start () { run(); }
	void quit () { };
private slots:
	void dealWithClient();
private:
	QTcpServer* tcpServer;
	bool mTerminate;

	QString mHost;
	int mPort;

	QSemaphore * fileLock;
};

#endif
