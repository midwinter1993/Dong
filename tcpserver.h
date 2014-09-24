#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QObject>
#include "tcpthread.h"

class TcpServer:public QTcpServer
{
	Q_OBJECT
public:
	TcpServer(QObject *parent = 0);
	~TcpServer(){};

signals:
	void dataReadSignal();

public slots:
	void disconnectedSlot();

protected:
	void incomingConnection(int socketDescriptor);

private:
	TcpThread *tcpThread;
};

#endif
