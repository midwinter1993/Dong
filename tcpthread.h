#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include "pca.h"
#include <QThread>
#include <QTcpSocket>

class DataProcessor;

class TcpThread:public QThread
{
	Q_OBJECT
public:
	TcpThread(int sockDpt, QObject *parent = 0);
	~TcpThread();
	void run();

signals:
	void dataReadSignal();
	void disconnectedSignal();

private:
	QTcpSocket *client;
	int socketDescriptor;
	DataProcessor *dataPro;
};

/* 实现多线程的一个trick：利用一个中间量，将这个中间量moveToThread，来实现真正的多线程
 * www.tuicool.com/articles/7777z2这个超链接里的3篇文章解释了Qt多线程的一些实现
 */

class DataProcessor:public QObject
{
	Q_OBJECT
public:
	DataProcessor(int sockDpt);

public slots:
	void readSockDataSlot();
	void disconnectedSockSlot();

signals:
	void dataFinishSignal();
	void disconnectedSockSignal();

private:
	PCA pca;
	int total_packet_sum;
	qint64 recv_packet_num;
	QByteArray sock_recv_buf;

	qint64 recv_byte_sum;

	QTcpSocket *client;
	int socketDescriptor;
	
	void dataProcess();
};
#endif
