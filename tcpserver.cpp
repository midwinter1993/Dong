#include "tcpserver.h"
#include "tcpthread.h"
#include <QTcpServer>

TcpServer::TcpServer(QObject *parent):
	QTcpServer(parent)
{
	
}

void TcpServer::incomingConnection(int socketDescriptor)
{
	tcpThread = new TcpThread(socketDescriptor, this);
	connect(tcpThread, SIGNAL(dataReadSignal()), this, SIGNAL(dataReadSignal()));
	connect(tcpThread, SIGNAL(disconnectedSignal()), this, SLOT(disconnectedSlot()));

	tcpThread->start();
	qDebug() << "tcpserver create thread" << QThread::currentThreadId();
}

void TcpServer::disconnectedSlot()
{
	qDebug() << "tcpServer disconnected\n";
	if (tcpThread->isRunning())
	{
		tcpThread->exit();	
	}
}
