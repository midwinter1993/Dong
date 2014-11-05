#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QThread>
#include <QTcpSocket>

class TmpObject;

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

public slots:
	void readDataSlot();
	// void disconnectedSlot();

private:
	QTcpSocket *client;
	int socketDescriptor;
	TmpObject *tmpObj;
};

/* 实现多线程的一个trick：利用一个中间量，将这个中间量moveToThread，来实现真正的多线程
 * www.tuicool.com/articles/7777z2这个超链接里的3篇文章解释了Qt多线程的一些实现
 */

class TmpObject:public QObject
{
	Q_OBJECT
public:
	char *buf;
	TmpObject(int sockDpt);

public slots:
	void readDataSlot_tmpObj();
	void disconnectedSlot_tmpObj();

signals:
	void dataReadSignal_tmpObj();
	void disconnectedSignal_tmpObj();

private:
	QTcpSocket *client;
	int socketDescriptor_tmpObj;
	
	void dataProcess();
};
#endif
