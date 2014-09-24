#include "tcpthread.h"
#include "def.h"
#include <QTcpSocket>
#include <QtGlobal>
#include <QThread>

//buffer for receive the data through socket
char socket_rec_buf[SOCKET_REC_BUF_SIZE];

TcpThread::TcpThread(int sockDpt, QObject *parent):
	QThread(parent)
{
	socketDescriptor = sockDpt;
	tmpObj = NULL;
	qDebug() << "tcpthread created" << QThread::currentThreadId();
}

TcpThread::~TcpThread()
{
	// if (!tmpObj)
		// delete tmpObj;
}

void TcpThread::run()
{
	tmpObj = new TmpObject(this->socketDescriptor);
	//-------------这是关键--------------------
	tmpObj->moveToThread(this);             //|
	//-----------------------------------------
	//这里的connect用了BlockingQueuedConnection
	connect(tmpObj, SIGNAL(dataReadSignal_tmpObj()), this, SIGNAL(dataReadSignal()), Qt::BlockingQueuedConnection);
	connect(tmpObj, SIGNAL(disconnectedSignal_tmpObj()), this, SIGNAL(disconnectedSignal()));
	qDebug() << "tcpthread run" << QThread::currentThreadId();
	exec();
}

void TcpThread::readDataSlot()
{

	if (client->bytesAvailable() < MSG_SIZE)
		return;
	// Q_ASSERT(client->read(buf, MSG_SIZE) == MSG_SIZE);
	client->read(socket_rec_buf, MSG_SIZE);
	// qDebug() << "temp " << temp << '\n';
	emit this->dataReadSignal();
}

TmpObject::TmpObject(int sockDpt)
{
	socketDescriptor_tmpObj = sockDpt;
	client = new QTcpSocket(this);
	if (!client->setSocketDescriptor(socketDescriptor_tmpObj))
	{
		Q_ASSERT(0);
	}
	client->setReadBufferSize(SOCKET_BUF_SIZE);
	
	connect(client, SIGNAL(readyRead()), this, SLOT(readDataSlot_tmpObj()));
	connect(client, SIGNAL(disconnected()), this, SLOT(disconnectedSlot_tmpObj()));
	qDebug() << "TmpObject created" << QThread::currentThreadId();
}

void TmpObject::readDataSlot_tmpObj()
{
	// qDebug() << "receive data thread ID: " << QThread::currentThreadId();
	//qDebug() << "bytes available: " << client->bytesAvailable() << '\n';
	
	quint64 tmp = client->bytesAvailable();
	qDebug() << "client bytesAvailable-------------->" << tmp << '\n';
	if (tmp < MSG_SIZE || tmp % MSG_SIZE != 0)
		Q_ASSERT(0);

	client->read(socket_rec_buf, tmp);
	*(quint64 *)(socket_rec_buf + SOCKET_REC_BUF_SIZE - 9) = tmp;

	emit this->dataReadSignal_tmpObj();
}

void TmpObject::disconnectedSlot_tmpObj()
{
	// delete client;
	qDebug() << "tmpObj disconnect\n";
	emit disconnectedSignal_tmpObj();
}
